#include "VMrate.h"

double VMrate::yield_function(double V, double, const double *z)
{
    double eps_el = elastic->Eps_el(V) - z[0];
    return yield_function(V, eps_el);
}

double VMrate::yield_strength(double V, double, const double *z)
{
    return yield_strength(V,z);
}

int VMrate::Rate(double V, double, const double *z, double *zdot)
{
    zdot[0] = 0.;
    zdot[1] = 0.;
    double eps_el = elastic->Eps_el(V) - z[0];
    double deps;
    if( eps_el < -epsy_el )
        deps = -(epsy_el+eps_el);
    else if( epsy_el < eps_el )
        deps = eps_el - epsy_el;
    else
        return 2;      // ExtEOS::n 
    zdot[0] = (eps_el<0) ? -deps*z[1] : deps*z[1];
    zdot[1] = pow(deps/epsy_el,n1)/tau1 * z[1];
    return 2;       // ExtEOS::n
}

int VMrate::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0;
    double eps_el = elastic->Eps_el(V);
    double epsz_el = eps_el - z[0];
    double deps = (epsz_el<0) ? -(epsz_el+epsy_el) : epsz_el-epsy_el;
    if( deps <= 0. )
        return 0;   // elastic region
    dt = 1./max(z[1],pow(deps/epsy_el,n1)/tau1);
    return 2;       // Stiff Integrate available
}

int VMrate::Integrate(double V, double, double *z, double dt)
{
    if( dt == 0. )
        return 0;
    if( dt < 0. )
        return 1;
    double eps_el = elastic->Eps_el(V);
    double epsz_el = eps_el - z[0];
    double epsy_pl, deps;
    if( epsz_el < -epsy_el )
    {
        epsy_pl = eps_el + epsy_el;
        deps = -(epsy_el+epsz_el);
    }
    else if( epsy_el < epsz_el )
    {
        epsy_pl = eps_el - epsy_el;
        deps = epsz_el - epsy_el;
    }
    else
        return 0;
    double epsn = pow(deps/epsy_el, n1)/(n1*tau1*z[1]);
    double tinv_inf = z[1]*(1.+epsn);
    double expdt = exp( -n1*dt*tinv_inf);
    z[1] *= (1.+epsn)/(1.+epsn*expdt);
    double f = (1.+epsn)*expdt/(1.+epsn*expdt);
    double dz = deps*pow(f,1./n1);
    z[0] = (epsy_el<epsz_el) ? epsy_pl - dz : epsy_pl + dz;
    return 0;
}

int VMrate::Equilibrate(double V, double, double *z)
{
    double eps_el = elastic->Eps_el(V);
    double epsz_el = eps_el - z[0];
    double epsy_pl, deps;
    if( epsz_el < -epsy_el )
    {
        epsy_pl = eps_el + epsy_el;
        deps = -(epsy_el+epsz_el);
    }
    else if( epsy_el < epsz_el )
    {
        epsy_pl = eps_el - epsy_el;
        deps = epsz_el - epsy_el;
    }
    else
        return 0;
    z[0]  = epsy_pl ;
    z[1] += pow(deps/epsy_el, n1)/(n1*tau1);
    return 0;
}
