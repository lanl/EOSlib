#include "StiffIdealHEav.h"

int StiffIdealHEav::Rate(double V, double e, const double *z, double *zdot)
{
    if( AvArrheniusRate::Rate(V,e,z,zdot) )
        return -1;      // error        
    return 2;           // number of IDOF variables
}

int StiffIdealHEav::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 1.e30;         // ignore time step
    if( z[0] >= 1. )
        return 0;       // all products, no constraint
    if( AvArrheniusRate::TimeStep(V,e,z,dt) )
        return -1;      // error
    if( dt == 0.0 )
        dt = 1.e30;     // ignore time step
    return 3;           // time step for Rate(), Integrate() available
}

int StiffIdealHEav::Integrate(double V, double e, double *z, double dt)
{
    return AvArrheniusRate::Integrate(V,e,z,dt);
}

int StiffIdealHEav::Equilibrate(double V, double e, double *z)
{
    if( HE == NULL )
        return 1;
    z[0] = 1.;                  // all products
    z[1] = (e+HE->Q)/HE->Cv2;   // products T
    return 0;
}

double StiffIdealHEav::Dt(double V, double e, const double *z, double lambda)
{
    return AvArrheniusRate::Dt(V,e,z,lambda);
}
