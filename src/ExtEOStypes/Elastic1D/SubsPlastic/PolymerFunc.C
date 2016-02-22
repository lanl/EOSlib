#include "Polymer.h"

double Polymer::yield_function(double V, double, const double *z)
{
    double eps_el = elastic->Eps_el(V) - z[0];
    return yield_function(V, eps_el);
}

double Polymer::yield_strength(double V, double, const double *)
{
    return yield_strength(V);
}

double Polymer::Tau(double eps_pl, double V)
{
    return tau0/(1.+pow(abs(eps_pl-epsy_pl(V))/eps0,n));
}

static double eps_tol = 1.e-3;
int Polymer::Rate(double V, double, const double *z, double *zdot)
{
    zdot[0] = 0.;
    if( V > V_ref )
    {
        if( V > 1.001*V_ref )
            return -1;   // Not Yet Implemented
        V = V_ref;
    }
    double eps_el = elastic->Eps_el(V) - z[0];
    if( eps_el > 0.+eps_tol )
        return -1;   // Not Yet Implemented
    double deps = z[0]-epsy_pl(V);
    if( deps > 0. )    
        zdot[0] = -deps*(1.+pow(deps/eps0,n))/tau0;
    return 1;       // ExtEOS::n
}

int Polymer::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0;
    if( V > V_ref )
    {
        if( V > 1.001*V_ref )
            return -1;   // Not Yet Implemented
        V = V_ref;
    }
    double eps_el = elastic->Eps_el(V) - z[0];
    if( eps_el > 0.+eps_tol )
        return -1;   // Not Yet Implemented
    double deps = z[0]-epsy_pl(V);
    if( deps <= 0. )
        return 0;   // elastic region
    dt = tau0/(1.+pow(deps/eps0,n));    // 1 e-fold
    return 2;       // Stiff Integrate available
}

int Polymer::Integrate(double V, double, double *z, double dt)
{
    if( dt == 0. )
        return 0;
    if( dt < 0. )
        return 1;
    if( V > V_ref )
    {
        if( V > 1.001*V_ref )
            return 1;   // Not Yet Implemented
        V = V_ref;
    }
    double eps_el = elastic->Eps_el(V) - z[0];
    if( eps_el > 0.+eps_tol )
        return 1;   // Not Yet Implemented
    double epsY = epsy_pl(V);
    if( frozen )
    {
        double deps = z[0]-epsY;
        if( deps > 0 )
        {
            double denom = pow(deps/eps0,n)*(1.-exp(-n*dt/tau0));
            denom = pow(1.+denom,1./n);
            z[0] = epsY + deps*exp(-dt/tau0)/denom;
        }
    }
    else
        z[0] = epsY;
    return 0;
}

int Polymer::Equilibrate(double V, double, double *z)
{
    
    if( V > V_ref )
    {
        if( V > 1.001*V_ref )
            return 1;   // Not Yet Implemented
        V = V_ref;
    }
    double eps_el = elastic->Eps_el(V) - z[0];
    if( eps_el > 0.+eps_tol )
        return 1;   // Not Yet Implemented
    z[0] = epsy_pl(V);
    return 0;
}
