#include "IdealHE.h"

int IdealHE::Rate(double V, double e, const double *z, double *zdot)
{
    if( ArrheniusRate::Rate(V,e,z,zdot) )
        return -1;      // error        
    return 1;           // number of IDOF variables
}

//
double IdealHE::Dt(double V, double e, const double *z, double lambda)
{
    return ArrheniusRate::Dt(V,e,z,lambda);    
}

int IdealHE::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 1.e30;         // ignore time step
    if( z[0] >= 1. )
        return 0;       // all products, no constraint
    if( ArrheniusRate::TimeStep(V,e,z,dt) )
        return -1;      // error
    if( dt == 0.0 )
        dt = 1.e30;     // ignore time step
    return 3;           // time step for Rate(), Integrate() available
}

int IdealHE::Integrate(double V, double e, double *z, double dt)
{
    return ArrheniusRate::Integrate(V,e,z,dt);
}

int IdealHE::Equilibrate(double V, double e, double *z)
{
    z[0] = 1.;  // all products
    return 0;
}
