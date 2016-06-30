#include <LocalMath.h>

#include "BKWwave.h"


Deflagration *BKW::deflagration(const HydroState &state, double P0)
{
    Deflagration *S = new BKWdeflagration(*this);
    if( S->Initialize(state,P0) )
    {
        delete S;
        S = NULL;
        EOSerror->Log("BKW::deflagration", __FILE__, __LINE__, this,
               "Initialize failed");
    }
    return S;
}

//

BKWdeflagration::BKWdeflagration(BKW &gas, double pvac) : Deflagration(gas,pvac)
{
    P0 = EOS::NaN;
    rel_tol = 1e-6;
}

BKWdeflagration::~BKWdeflagration()
{
    // Null
}

int BKWdeflagration::InitCJ()
{
    double Vmin = V0;
    P1 = eos->P(Vmin,e0);
    double fmin = P1 - P0;
    if( fmin <= 0 )
    {
        P0 = EOS::NaN;
        eos->ErrorHandler()->Log("BKWdeflagration::InitCJ", __FILE__, __LINE__,
                eos, "fmin <= 0\n" );
        return 1;
    }
    double Vmax;
    double fmax;
    int count;
    for( count=10; count>0; count-- )
    {
        double dPdV = eos->c2(Vmin,e0-P0*(Vmin-V0))/Vmin/Vmin;
        double dV = 2*(P1-P0)/dPdV;
        Vmax = Vmin + dV;
        P1 = eos->P(Vmax,e0-P0*(Vmax-V0));
        fmax = P1 - P0;
        if( fmax <= 0. )
            break;
        Vmin = Vmax;
        fmin = fmax;
    }
    if( count <= 0  )
    {
        P0 = EOS::NaN;
        eos->ErrorHandler()->Log("BKWdeflagration::InitCJ", __FILE__, __LINE__,
                eos, "failed to init Vp0\n" );
        return 1;
    }
    fzero = VMIN;
    Vp0 = zero(Vmin,fmin,Vmax,fmax);
    if( Status() )
    {
        P0 = EOS::NaN;
        eos->ErrorHandler()->Log("BKWdeflagration::InitCJ", __FILE__, __LINE__,
                eos, "zero for VMIN failed with status %s\n", ErrorStatus() );
        return 1;
    }
    ep0 = e0 + P0*(V0 - Vp0);

  
    fzero = CJ;
    Vmin = Vp0;
    fmin = eos->c(Vp0,ep0);
    P1 = P0;
    for( count=10; count>0; count-- )
    {
        double dPdV = eos->c2(Vmin,e0)/Vmin/Vmin;
        Vmax = Vmin + P1/dPdV;
        fmax = f(Vmax);
        if( fmax <= 0. )
            break;
        Vmin = Vmax;
        fmin = fmax;
    }
    if( count <= 0  )
    {
        P0 = EOS::NaN;
        eos->ErrorHandler()->Log("BKWdeflagration::InitCJ", __FILE__, __LINE__,
                eos, "failed to init Vp0\n" );
        return 1;
    }
    Vcj = zero(Vmin,fmin,Vmax,fmax);
    if( Status() )
    {
        P0 = EOS::NaN;
        eos->ErrorHandler()->Log("BKWdeflagration::InitCJ", __FILE__, __LINE__,
                eos, "zero for CJ failed with status %s\n", ErrorStatus() );
        return 1;
    }
    (void) f(Vcj);
    Pcj = P1;
    ecj = e1;
    Dcj = D;
    ucj = (1.-Vcj/V0)*Dcj;
    return 0;
}


double BKWdeflagration::f(double var)
{
    V1 = var;
    double dV = V0 - V1;
    switch (fzero)
    {
    case VMIN:
    {
        e1 = e0 + P0*dV;
        P1 = eos->P(V1,e1);
        return P1 - P0;
    }
    case CJ:
    {
        P1 = eos->P(V1,e0);
        double G = eos->Gamma(V1,e0);   // Gamma independent of e
        double de = (P1+P0)*dV/(2. - dV*G/V1);
        e1 = e0 + de;
        P1 += (G/V1)*de;
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




int BKWdeflagration::P(double p1,  int dir, WaveState &wave)
{
    if( std::isnan(P0) )
        return 1;
    if( abs(Pcj - p1) < rel_tol*Pcj  )
        return CJwave( dir, wave );
    if( p1 < Pcj || P0 < p1 )
        return 1;
    
    fzero = pressure;
    Pav = 0.5*(p1+P0);
    
    double V = inverse(p1, Vp0, Vcj);
    if( Status() )
    {
        eos->ErrorHandler()->Log("BKWdeflagration::P", __FILE__, __LINE__,
             eos, "inverse failed with status: %s\n", ErrorStatus() );
        return 1;
    }  
    (void) f(V);                // evaluate state

// set shock state  
    wave.V  = V1;
    wave.e  = e1;
    wave.P  = p1;
    D = dir * V0 * sqrt( (p1-P0)/(V0-V1) );
    wave.u  = u0 + (1. - V1/V0)*D;
    wave.us = u0 + D;  
        
    return 0;
}

int BKWdeflagration::u(double u1,  int dir, WaveState &wave)
{
    if( std::isnan(P0) )
        return 1;
    wave.u = u1;
    u1 = dir*(u1-u0);
    if( abs(ucj - u1) < -rel_tol*ucj  )
        return CJwave( dir, wave );
    if( abs(u1) < rel_tol*ucj  )
    {
        wave.V = Vp0;
        wave.e = ep0;
        wave.P = P0;
        wave.u = u0;
        wave.us = u0;
        return 0;
    }        
    if( u1 < ucj || 0. < u1 )
        return 1;
    fzero = velocity;
    du2 = sqr(u1);   // mass flux       
    double V = inverse(du2, Vp0, Vcj);
    if( Status() )
    {
        eos->ErrorHandler()->Log("BKWdeflagration::u", __FILE__, __LINE__,
             eos, "inverse failed with status: %s\n", ErrorStatus() );
        return 1;
    }  
    (void) f(V);                // evaluate state
// set shock state  
    wave.V  = V1;
    wave.e  = e1;
    wave.P  = P1;
    wave.us = u0 + V0/(V0-V1) * u1;          
    return 0;
}

int BKWdeflagration::u_s(double us, int dir, WaveState &wave)
{
    if( std::isnan(P0) )
        return 1;
    wave.us = us;        
    us = dir*(us-u0);
    if( abs(Dcj - us) < rel_tol*Dcj  )
        return CJwave( dir, wave );
    if( abs(us) < rel_tol*Dcj  )
    {
        wave.V = Vp0;
        wave.e = ep0;
        wave.P = P0;
        wave.u = u0;
        wave.us = u0;
        return 0;
    }        
    if( us < 0. || Dcj < us )
        return 1;
    fzero = wave_speed;
    m2 = sqr(us/V0);   // mass flux       
    double V = zero(Vp0,Vcj);
    if( Status() )
    {
        eos->ErrorHandler()->Log("BKWdeflagration::u_s", __FILE__, __LINE__,
             eos, "inverse failed with status: %s\n", ErrorStatus() );
        return 1;
    }  
    (void) f(V);                // evaluate state
    wave.V  = V1;
    wave.e  = e1;
    wave.P  = P1;
    us *= dir;
    wave.u  = u0 + (1. - V1/V0)*us;
    return 0;
}

int BKWdeflagration::V(double v1,  int dir, WaveState &wave)
{
    if( std::isnan(P0) )
        return 1;
    if( abs(Vcj - v1) < rel_tol*Vcj )
        return CJwave( dir, wave );
    if( abs(Vp0-v1) < rel_tol*Vp0  )
    {
        wave.V = Vp0;
        wave.e = ep0;
        wave.P = P0;
        wave.u = u0;
        wave.us = u0;
        return 0;
    }        
    if( v1 < Vp0 || Vcj < v1 )
        return 1;
    V1 = v1;
    double dV = V0 - V1;
    double G = eos->Gamma(V1,e0);   // Gamma independent of e
    double denom = 2. - dV*G/V1;
    P1 = eos->P(V1,e0);
    double de = (P1+P0)*dV/denom;
    e1 = e0 + de;
    P1 += (G/V1)*de;
    
    wave.V  = V1;
    wave.P  = P1;
    wave.e  = e1;
    D = dir * V0 * sqrt( (P1-P0)/dV );
    wave.us = u0 + D;   
    wave.u  = u0 + (dV/V0)*D;

    return 0;
}
