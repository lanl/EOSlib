#include <PTequilibrium.h>
#include <IgnitionGrowthRT.h>

int IgnitionGrowthRT::Rate(double V, double e, const double *z, double *zdot)
{
    //double Ps = z[0];
    //double ts = z[1];
    // z[2] = lambda
    // z[3] = s
    zdot[3] = 0.0;  // reaction scale function not used
    return IgnitionGrowth::Rate(V, e, z+2, zdot+2);
}

int IgnitionGrowthRT::TimeStep(double V, double e, const double *z, double &dt)
{
    return IgnitionGrowth::TimeStep(V, e, z+2, dt);
}

int IgnitionGrowthRT::Integrate(double V, double e, double *z, double dt)
{
   return IgnitionGrowth::Integrate(V, e, z+2, dt);
}

double IgnitionGrowthRT::Dt(double V, double e, const double *z, double lambda)
{
   return IgnitionGrowth::Dt(V, e, z+2, lambda);
}
