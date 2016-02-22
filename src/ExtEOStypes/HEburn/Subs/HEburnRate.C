#include "HEburn.h"

int HEburn::Rate(double V, double e, const double *z, double *zdot)
{
    if( rate->Rate(V,e,z,zdot) )
        return -1;      // error        
    return ExtEOS::n;   // number of IDOF variables
}

double HEburn::Dt(double V, double e, const double *z, double lambda)
{
    return rate->Dt(V,e,z,lambda);    
}


int HEburn::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 1.e30;         // ignore time step
    //HotSpotCC_Rate has sequence of two rates
    //if( z[ilambda] >= 1. )
    //    return 0;       // all products, no constraint
    int type = rate->TimeStep(V,e,z,dt);
    if( type < 0 )
        return -1;      // error
    if( type == 0 || type == 2 )
        dt = 1.e30;     // ignore time step
    return type;        // time step for Rate(), Integrate() available
}

int HEburn::Integrate(double V, double e, double *z, double dt)
{
    return rate->Integrate(V,e,z,dt);
}

int HEburn::Equilibrate(double V, double e, double *z)
{
    z[ilambda] = 1.;    // all products
    return 0;
}
