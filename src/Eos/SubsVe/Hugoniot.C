#include <LocalMath.h>

#include "EOS.h"
#include "Hugoniot_gen.h"

Hugoniot::~Hugoniot()
{
    // NULL
}

int Hugoniot::Initialize(const HydroState &state)
{
    return Wave::Initialize(state);
}

//

Hugoniot_generic::Hugoniot_generic(EOS &e, double pvac)
                                : Hugoniot(e,pvac), S(NULL), s(NaN)
{
    OneDFunction::rel_tolerance = eos->OneDFunc_rel_tol;
    OneDFunction::abs_tolerance = eos->OneDFunc_abs_tol;
}


Hugoniot_generic::~Hugoniot_generic()
{
    delete S;
}

double Hugoniot_generic::f(double var)
{
    double dV;
    double value;
    switch (type)
    {
    case pressure:
        V1 = var;
        e1 = state0.e + p_av*(state0.V - V1);
        p1 = eos->P(V1,e1);
        value = p1;
        break;
    case velocity:
        V1 = var;
        e1 = state0.e + 0.5*du*du + state0.P*(state0.V - V1);
        p1 = eos->P(V1,e1);
        value = (p1-state0.P)*(state0.V - V1);   // du^2
        break;
    case wave_speed:
        V1 = var;
        dV = state0.V - V1;
        e1 = state0.e + 0.5*m2*dV*dV + state0.P*dV;
        p1 = eos->P(V1,e1);
        value = (p1-state0.P)-m2*dV;
        break;
    case specificV:
        e1 = var;
        p1 = eos->P(V1,e1);
        value = e1-state0.e - 0.5*(p1+state0.P)*(state0.V - V1);
        break;
    }
    return eos->NotInDomain(V1,e1) ? EOS::NaN : value;
}

int Hugoniot_generic::Vbound_P(double p, double &Vmin, double &fmin,
                                         double &Vmax, double &fmax)
{
    double Vlimit = 0;  // limit for domain of EOS

    Vmax = state0.V;
    fmax = state0.P;    // fmax < p
    Vmin = state0.V*(1-state0.V*(p-state0.P)/sqr(state0.c));
    Vmin = max(0.5*(Vlimit+Vmax), Vmin);
    
    int count = 40;
    for( ;count--; Vmin = 0.5*(Vlimit+Vmax)  )
    {
        fmin = f(Vmin);
        if( isnan(fmin) || p1 <= state0.P )
        { // outside domain of EOS
            Vlimit = Vmin;
            continue;
        }
        /**********/
        // check whether f(V) is monotonic
        // f monotonic if Gamma >= 0
        double Gamma = eos->Gamma(V1,e1);
        double c2    = eos->c2(V1,e1);
        double dPdV  = -c2/(V1*V1)+ Gamma*p1/V1;
        double dfdV = dPdV - Gamma*p_av/V1;
        if( dfdV > 0.0 )
        {
            Vlimit = Vmin;
            continue;            
        }
        /***********/
        if( fmin >= p )
            return 0;
        fmax = fmin;
        Vmax = Vmin;
    }
    return 1;   // failure
}


int Hugoniot_generic::P(double p, int dir, WaveState &shock)
{
// Hugoniot point specified by p
    if( p <= state0.P )
    {
        // check for round-off
        if( state0.P - p < 1e-4 * state0.c*state0.c/state0.V )
        {
            shock.V = state0.V;
            shock.e = state0.e;
            shock.P  = state0.P;
            shock.u  = state0.u;
            shock.us = state0.u + dir * state0.c;
            return 0;
        }
        eos->ErrorHandler()->Log("Hugoniot_generic::P", __FILE__, __LINE__,
             eos, "p < P0 (%f,%f)\n", p, state0.P);
        shock.V = EOS::NaN;
        return -1;
    }
    p_av = 0.5*(state0.P+p);
    double dp = p - state0.P;
// check for weak shocks
    double K = state0.c*state0.c/state0.V;  // bulk modulus
    if( dp < 1e-6 * K )
    {
    // On isentrope: (P-P0)/(rho_0 c0^2) = -dV/V0 + G(dV/V0)^2 + . . .
    // Assume, G = O(1)
    // then error is order (dV/V0)^2 = (dp/K)^2 < 1e-6
        double V  = (1. - dp/K) * state0.V;
        double dV = (dp/K)*state0.V;
        double e  = state0.e + 0.5*(p+state0.P)*dV;
    // predictor-corrector (entropy change is 3rd order)
        double c2 = eos->c2(V,e);
        double VKinv = 0.5*(sqr(state0.V/state0.c) + V*V/c2);
        dV = dp*VKinv;
    
        shock.V = state0.V - dV;
        shock.e = state0.e + 0.5*(p+state0.P)*dV;
        shock.P  = p;
        
        double rhoc_inv = sqrt(VKinv);
        shock.u  = state0.u + dir * dp*rhoc_inv;
        shock.us = state0.u + dir * state0.V/rhoc_inv;
        return 0;   
    }
// use zero finder
    type = pressure;
    double Vmin, Vmax;
    double fmin,fmax;
    if( Vbound_P(p, Vmin,fmin, Vmax,fmax) )
    {
        eos->ErrorHandler()->Log("Hugoniot_generic::P", __FILE__, __LINE__,
             eos, "Hugoniot_generic::Vbound_P, failed\n");
        return -1;
    }
// solve Hugoniot equation
    double V = inverse(p, Vmin, fmin, Vmax, fmax);
    if( Status() )
    {
        eos->ErrorHandler()->Log("Hugoniot_generic::P", __FILE__, __LINE__,
             eos, "inverse failed with status: %s\n", ErrorStatus());
        return -1;
    }
    (void) f(V);                // evaluate state
// set shock state  
    double u_s = dir * state0.V * sqrt( dp/(state0.V-V1) );
    shock.V  = V1;
    shock.e  = e1;
    shock.u  = state0.u + (1 - V1/state0.V)*u_s;
    shock.P  = p1;
    shock.us = state0.u + u_s;  
        
    return 0;
}   

//

int Hugoniot_generic::Vbound_us(double us, double &Vmin, double &fmin,
                                          double &Vmax, double &fmax)
{
    double Vlimit = 0;  // limit for domain of EOS

    // max compression ratio = 1 + 2/Gamma
    Vmin = 0.5*state0.V*( 1 + 1/(1. + 2/eos->Gamma(state0.V,state0.e)) );
    if( Vmin >= Vmax )
        Vmin = 0.5*state0.V;
    
    int count = 40;
    for( ;count--; Vmin = 0.5*(Vlimit+Vmax)  )
    {
        fmin = f(Vmin);
        if( isnan(fmin) || p1 <= state0.P )
        { // outside domain of EOS
            Vlimit = Vmin;
            continue;
        }
        if( fmin >= 0 )
            return 0;
        fmax = fmin;
        Vmax = Vmin;
    }
    return 1;   // failure
}


int Hugoniot_generic::u_s(double us, int dir, WaveState &shock)
{
// Hugoniot point specified by us
    double dus = dir*(us-state0.u);
    if( dus <= state0.c )
    {
        // check for round-off
        if( dus > (1-1e-4) * state0.c )
        {
            shock.V = state0.V;
            shock.e = state0.e;
            shock.P  = state0.P;
            shock.u  = state0.u;
            shock.us = state0.u + dir * state0.c;
            return 0;
        }
        eos->ErrorHandler()->Log("Hugoniot_generic::u_s",
             __FILE__, __LINE__, eos,
             "failed, subsonic wave speed\n");
        return -1;
    }
    m2 = sqr((us-state0.u)/state0.V);   // mass flux    
    type = wave_speed;
    double Vmin, Vmax;
    double fmin, fmax;
    Vmax = 0.99*state0.V;
    fmax = f(Vmax);
    if( fmax >= 0 )
    {
        if( isnan(s) )
            s = 0.5*eos->FD(state0.V,state0.e);
        if( isnan(s) || s < 0 )
            return -1;
        double D = dir*(us -state0.u);
        double u = (D - state0.c)/s;
        double dV = (u/D) * state0.V;
        shock.V  = state0.V - dV;
        shock.e  = state0.e + 0.5*m2*dV*dV + state0.P*dV;
        shock.P  = state0.P + u*D/state0.V;
        shock.u  = state0.u + dir*u;
        shock.us = us;
        return 0;   
    }
// use zero finder
    if( Vbound_us(us, Vmin,fmin, Vmax,fmax) )
    {
        eos->ErrorHandler()->Log("Hugoniot_generic::u_s", __FILE__, __LINE__,
                eos, "Vbound_us, failed\n");
        return -1;
    }
// solve Hugoniot equation
    double V = zero(Vmin, fmin, Vmax, fmax);
    if( Status() )
    {
        eos->ErrorHandler()->Log("Hugoniot_generic::u_s", __FILE__, __LINE__,
                eos, "zero failed with status: %s\n", ErrorStatus());
        return -1;
    }
    (void) f(V);                // evaluate state
// set shock state  
    shock.V  = V1;
    shock.e  = e1;
    shock.P  = p1;
    shock.us = us;  
    shock.u  = state0.u + (1 - V1/state0.V)*(us-state0.u);
    return 0;
}   

//

int Hugoniot_generic::Vbound_u(double u, double &Vmin, double &fmin,
                                          double &Vmax, double &fmax)
{
    double Vlimit = 0.;  // limit for domain of EOS

    Vmax = state0.V;
    fmax = 0.0;
    Vmin = state0.V*(1.- du/state0.c);
    Vmin = max(0.5*(Vlimit+Vmax), Vmin);
    
    int count = 40;
    for( ;count--; Vmin = 0.5*(Vlimit+Vmax)  )
    {
        fmin = f(Vmin);
        if( isnan(fmin) || p1 <= state0.P )
        { // outside domain of EOS
            Vlimit = Vmin;
            continue;
        }
        /****/
        // Check whether f(V) is monotonic
        double Gamma = eos->Gamma(V1,e1);
        double c2    = eos->c2(V1,e1);
        double dfdV = -p1 +((p1-state0.P)*Gamma/V1-c2/(V1*V1))*(state0.V-V1);
        if( dfdV > 0.0 )
        {
            Vlimit = Vmin;
            continue;            
        }
        /****/
        if( fmin >= du*du )
            return 0;
        fmax = fmin;
        Vmax = Vmin;
    }
    return 1;   // failure
}


int Hugoniot_generic::u(double u, int dir, WaveState &shock)
{
// Hugoniot point specified by u
    du = dir*(u-state0.u);
    if( du <= 0 )
    {
        // check for round-off
        if( -du < 1e-4 * state0.c )
        {
            shock.V = state0.V;
            shock.e = state0.e;
            shock.P  = state0.P;
            shock.u  = state0.u;
            shock.us = state0.u + dir * state0.c;
            return 0;
        }
        eos->ErrorHandler()->Log("Hugoniot_generic::u",
            __FILE__, __LINE__, eos,
            "failed, du < 0\n");
        return -1;
    }

// check for weak shocks
    if( du < 1e-6 * state0.c )
    {
        double dV = du*state0.V/state0.c;
        shock.V  = state0.V - dV;
        shock.e  = state0.e + 0.5*du*du + state0.P*dV;
        shock.P  = eos->P(shock.V, shock.e);
        shock.u  = u;
        shock.us = state0.u
                   + dir*0.5*(state0.c+sqrt(eos->c2(shock.V, shock.e)));
        return 0;   
    }
    
    type = velocity;
    
// find bounds
    double Vmin, Vmax;
    double fmin,fmax;
    
    if( Vbound_u(u, Vmin,fmin, Vmax,fmax) )
    {
        eos->ErrorHandler()->Log("Hugoniot_generic::u",
             __FILE__, __LINE__, eos,
             "Vbound_u, failed\n");
        return -1;
    }

// solve Hugoniot equation
    double V = inverse(du*du, Vmin, fmin, Vmax, fmax);
    if( Status() )
    {
        eos->ErrorHandler()->Log("Hugoniot_generic::u",
             __FILE__, __LINE__, eos,
             "inverse failed with status: %s\n", ErrorStatus() );
        return -1;
    }
    
    (void) f(V);                // evaluate state

// set shock state  
    shock.V  = V1;
    shock.e  = e1;
    shock.u  = u;
    shock.P  = p1;
    shock.us = state0.u +dir*state0.V*du/(state0.V-V1); 
        
    return 0;
}   

//

int Hugoniot_generic::e_bound(double v, double &e_min, double &fmin,
                                        double &e_max, double &fmax)
{
    if( S == NULL )
        S = eos->isentrope(state0);
    WaveState wave;
    if( S == NULL || S->V(v,RIGHT, wave) )
        return 1;
    e_min = wave.e;
    fmin = f(e_min);
    if( isnan(fmin)|| fmin > 0 )
        return 1;
    double dV = state0.V - V1;
    
    int count = 10;
    while( count-- )
    {
        double Gamma = eos->Gamma(V1,e_min);
        if( isnan(Gamma) )
            return 1;
        
        double den = 1 - 0.5*Gamma*dV/V1;
        if( den <= 0 )
            return 1;
        
        double de = -fmin/den;
        e_max = e_min + 2*de;
        fmax = f(e_max);
        if( fmax >= 0 )
            return 0;
        fmin = fmax;
        e_min = e_max;
    }
    return 1;   // failure
}


int Hugoniot_generic::V(double v, int dir, WaveState &shock)
{
// Hugoniot point specified by v
    if( v > state0.V )
    {
        // check for round-off
        if( v-state0.V < 1e-4 * state0.V )
        {
            shock.V = state0.V;
            shock.e = state0.e;
            shock.P  = state0.P;
            shock.u  = state0.u;
            shock.us = state0.u + dir * state0.c;
            return 0;
        }
        eos->ErrorHandler()->Log("Hugoniot_generic::V",
             __FILE__, __LINE__, eos,
             "failed, V > V0\n");
        return -1;
    }
    
    double dV = state0.V - v;
    if( dV < 0.01*state0.V )
    { // weak shock, u_s = c0 + s*u_p
        if( isnan(s) )
            s = 0.5*eos->FD(state0.V,state0.e);
        if( isnan(s) || s < 0 )
            return -1;
        double c0 = state0.c;
        double V0 = state0.V;
        double us = dir*c0/(1-s*dV/V0);
        double up = dV/V0 * us;
        
        shock.V  = v;
        shock.P  = state0.P + up*us/V0;
        shock.e  = state0.e + 0.5*(state0.P+shock.P)*dV;
        shock.us = state0.u + us;   
        shock.u  = state0.u + up;
        
        return 0;
    }
    
    type = specificV;
    V1 = v;
    
    double e_min, e_max;
    double fmin, fmax;
    
// find bounds
    if( e_bound(v, e_min,fmin, e_max,fmax) )
    {
        eos->ErrorHandler()->Log("Hugoniot_generic::V",
             __FILE__, __LINE__, eos,
             "failed, Vbound_us\n");
        return -1;
    }

// solve Hugoniot equation
    double e = zero(e_min, fmin, e_max, fmax);
    if( Status() )
    {
        eos->ErrorHandler()->Log("Hugoniot_generic::V",
             __FILE__, __LINE__, eos,
             "zero failed with status: %s\n", ErrorStatus() );
        return -1;
    }
    
    (void) f(e);                // evaluate state

// set shock state  
    shock.V  = V1;
    shock.e  = e1;
    shock.P  = p1;
    du = dir*sqrt((p1-state0.P)*dV);
    shock.u  = state0.u + du;
    shock.us = state0.u + state0.V/dV*du;   
        
    return 0;
}   

