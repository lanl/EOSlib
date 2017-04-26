#include "HEburn2.h"

int HEburn2::Rate(double V, double e, const double *z, double *zdot)
{
    if( rate->Rate(V,e,z,zdot) )
        return -1;      // error        
    return ExtEOS::n;   // number of IDOF variables
}

double HEburn2::Dt(double V, double e, const double *z,
                   double &lambda, double &lambda2)
{
    return rate->Dt(V,e,z, lambda,lambda2);    
}


int HEburn2::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 1.e30;         // ignore time step
    int type = rate->TimeStep(V,e,z,dt);
    if( type < 0 )
        return -1;      // error
    if( type == 0 || type == 2 )
        dt = 1.e30;     // ignore time step
    return type;        // time step for Rate(), Integrate() available
}

int HEburn2::Integrate(double V, double e, double *z, double dt)
{
    return rate->Integrate(V,e,z,dt);
}

int HEburn2::Equilibrate(double V, double e, double *z)
{
    return set_lambda(1.0,1.0, z);    // all products
}
