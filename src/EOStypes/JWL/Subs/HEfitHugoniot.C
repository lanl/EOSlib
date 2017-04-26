#include <LocalMath.h>

#include "EOS.h"
#include "HEfit.h"

HEfitdetonation::HEfitdetonation(HEfit &Gas, double pvac) : Detonation(Gas,pvac)
{
    P0 = EOS::NaN;
    rel_tol = 1e-6;
    V_min = 1./Gas.rho_max;
    V_max = 1./Gas.rho_min;
    e_CJ  = Gas.e_CJ;
}

HEfitdetonation::~HEfitdetonation()
{
    // Null
}

int HEfitdetonation::InitCJ()
{
    fzero = CJ;
    double fmin = f(V_min);
    if( std::isnan(fmin) )
        fmin = HUGE;
    double fmax = f(V_max);
    if( std::isnan(fmax) )
        fmax = -HUGE;
    Vcj = zero(V_min,fmin, V_max,fmax);
    if( Status() )
    {
        P0 = EOS::NaN;
        eos->ErrorHandler()->Log("HEfitdetonation::InitCJ", __FILE__, __LINE__,
                eos, "zero failed with status %s\n", ErrorStatus() );
        return 1;
    }
    (void) f(Vcj);
    Pcj = P1;
    ecj = e1;
    Dcj = D;
    ucj = (1-Vcj/V0)*Dcj;
    return 0;
}


double HEfitdetonation::f(double var)
{
    V1 = var;
    double dV = V0 - V1;
    switch (fzero)
    {
    case CJ:
    {
        P1 = eos->P(V1,e0);
        double omega = eos->Gamma(V1,e0);
        double de = (P1+P0)*dV/(2 - dV*omega/V1);
        e1 = e0 + de;
        P1 += (omega/V1)*de;
        D  = V0*sqrt((P1-P0)/dV);
        return eos->c(V1,e1) - (V1/V0)*D;   // u+c - D
    }
    case pressure:
        e1 = e0 + Pav*dV;
        P1 = eos->P(V1,e1);
        return P1;
    case velocity:
        e1 = e0 + 0.5*du2 + P0*dV;
        P1 = eos->P(V1,e1);
        return (P1-P0)*dV;                  // du2
    case wave_speed:
        e1 = e0 + 0.5*m2*dV*dV + P0*dV;
        P1 = eos->P(V1,e1);
        return (P1-P0)-m2*dV;
    }
}    




int HEfitdetonation::P(double p1,  int dir, WaveState &wave)
{
    if( std::isnan(P0) )
        return 1;
    if( std::abs(Pcj - p1) < rel_tol*Pcj  )
        return CJwave( dir, wave );
    if( p1 < Pcj )
        return 1;
    
    fzero = pressure;
    Pav = 0.5*(p1+P0);
    
    double V = inverse(p1, V_min, Vcj);
    if( Status() )
    {
        eos->ErrorHandler()->Log("HEfitdetonation::P", __FILE__, __LINE__,
             eos, "inverse failed with status: %s\n", ErrorStatus() );
        return 1;
    }  
    (void) f(V);                // evaluate state

// set shock state  
    wave.V  = V1;
    wave.e  = e1;
    wave.P  = p1;
    D = dir * V0 * sqrt( (p1-P0)/(V0-V1) );
    wave.u  = u0 + (1 - V1/V0)*D;
    wave.us = u0 + D;  
        
    return 0;
}

int HEfitdetonation::u(double u1,  int dir, WaveState &wave)
{
    if( std::isnan(P0) )
        return 1;
    u1 = dir*(u1-u0);
    if( std::abs(ucj - u1) < rel_tol*ucj  )
        return CJwave( dir, wave );
    if( u1 < ucj )
        return 1;
    
    fzero = velocity;
    du2 = sqr(u1);   // mass flux    
    
    double V = inverse(du2, V_min, Vcj);
    if( Status() )
    {
        eos->ErrorHandler()->Log("HEfitdetonation::u", __FILE__, __LINE__,
             eos, "inverse failed with status: %s\n", ErrorStatus() );
        return 1;
    }  
    (void) f(V);                // evaluate state

// set shock state  
    wave.V  = V1;
    wave.e  = e1;
    wave.P  = P1;
    wave.u  = u0 + u1;
    wave.us = u0 + V0/(V0-V1) * u1;  
        
    return 0;
}

int HEfitdetonation::u_s(double us, int dir, WaveState &wave)
{
    if( std::isnan(P0) )
        return 1;
    us = dir*(us-u0);
    if( std::abs(Dcj - us) < rel_tol*Dcj  )
        return CJwave( dir, wave );
    if( us < Dcj )
        return 1;
    
    fzero = wave_speed;
    m2 = sqr(us/V0);   // mass flux    
    
    double V = zero(V_min,Vcj);
    if( Status() )
    {
        eos->ErrorHandler()->Log("HEfitdetonation::u_s", __FILE__, __LINE__,
             eos, "inverse failed with status: %s\n", ErrorStatus() );
        return 1;
    }  
    (void) f(V);                // evaluate state

// set shock state  
    wave.V  = V1;
    wave.e  = e1;
    wave.P  = P1;
    us *= dir;
    wave.u  = u0 + (1 - V1/V0)*us;
    wave.us = u0 + us;  
        
    return 0;
}
int HEfitdetonation::w_u_s(double us, int dir, WaveState &wave)
{
    return 1; // To do
}

int HEfitdetonation::V(double v1,  int dir, WaveState &wave)
{
    if( std::isnan(P0) )
        return 1;
    if( std::abs(Vcj - v1) < rel_tol*Vcj )
        return CJwave( dir, wave );
    if( v1 <= V_min || Vcj < v1 )
        return 1;
       
    V1 = v1;
    double dV = V0 - V1;
    double omega = eos->Gamma(V1,e0);
    double denom = 2 - dV*omega/V1;
    P1 = eos->P(V1,e0);
    double de = (P1+P0)*dV/denom;
    e1 = e0 + de;
    P1 += (omega/V1)*de;
    
    wave.V  = V1;
    wave.P  = P1;
    wave.e  = e1;
    D = dir * V0 * sqrt( (P1-P0)/dV );
    wave.us = u0 + D;   
    wave.u  = u0 + (dV/V0)*D;

    return 0;
}

Detonation *HEfit::detonation(const HydroState &state, double P0)
{
    Detonation *S = new HEfitdetonation(*this);
    if( S->Initialize(state,P0) )
    {
        delete S;
        S = NULL;
        EOSerror->Log("HEfit::shock", __FILE__, __LINE__, this,
               "Initialize failed");
    }
    return S;
}

