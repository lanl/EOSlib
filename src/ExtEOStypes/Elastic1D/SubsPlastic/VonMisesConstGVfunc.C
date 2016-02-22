#include "VonMisesConstGV.h"
#include <OneDFunction.h>

double VonMisesConstGV::f(double eps_el)  // OneDFunction::f
{
    return Yfunction(eps_el);
}
//
double VonMisesConstGV::yield_function(double V, double e, const double *z)
{
    double eps_el = elastic->Eps_el(V) - z[0];
    return Yfunction(eps_el);
}

double VonMisesConstGV::yield_strength(double V, double e, const double *z)
{
    return Y0;
}

int VonMisesConstGV::Rate(double V, double e, const double *z, double *zdot)
{
    zdot[0] = 0.;
    double eps_el = elastic->Eps_el(V)-z[0];
    if( (eps_el < 0) ? eps_y_compression <= eps_el : eps_el <= eps_y_expansion )
        return 1;  // ExtEOS::n
    double Yf = Yfunction(eps_el);
    zdot[0] = 1.;
    if( Y0>0 && A2>0 )
        zdot[0] += A2*pow((Yf/Y0-1),n2);
    if( eps_y>0 && A1>0 )
        zdot[0] += A1*pow(abs(z[0])/eps_y,n1);
    zdot[0] *= 0.5*(Yf-Y0)/nu;
    if( eps_el < 0 )
        zdot[0] = -zdot[0];
    return 1; // ExtEOS::n
}

int VonMisesConstGV::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0;
    double eps_el = elastic->Eps_el(V)-z[0];
    if( (eps_el < 0) ? eps_y_compression <= eps_el : eps_el <= eps_y_expansion )
        return 0;       // Yf < Y, no constraint
    double f = 1.;
    if( Y0>0 && A2>0 )
        f += A2*pow((Yfunction(eps_el)/Y0-1),n2);
    if( eps_y>0 && A1>0 )
        f += A1*pow(abs(z[0])/eps_y,n1);
    dt = (nu/f)*(V/GV(eps_el)); // e-fold of relaxation = (0.5*(Yf-Y)/G)/|Rate|
    return 2;       // Stiff Integrate available
}

int VonMisesConstGV::Integrate(double V, double e, double *z, double dt)
{
    if( dt == 0 )
        return 0;
    if( dt < 0 )
        return 1;
    V1     = V;
    eps_V1 = elastic->Eps_el(V);
    double eps_el = eps_V1 - z[0];
    if( frozen )
    {
        int count = 25;    
        while( count-- )
        {
            if( Step(eps_el,dt)==0 || dt <= 0 )
            {
                z[0] = eps_V1 - eps_el;
                return 0;
            }
        }
    }
    z[0] = eps_V1 - ((eps_el<=0) ? eps_y_compression : eps_y_expansion);
    return 0;
}

int VonMisesConstGV::Step(double &eps_el, double &t)
{
    if( (eps_el < 0) ? eps_y_compression<=eps_el : eps_el<=eps_y_expansion )
        return 0;
    double Yf = Yfunction(eps_el);
    double f = 1.;
    if( Y0>0 && A2>0 )
        f += A2*pow((Yf/Y0-1),n2);
    if( eps_y>0 && A1>0 )
        f += A1*pow(abs(eps_V1-eps_el)/eps_y,n1);
    double G = GV(eps_el)/V1;
    double tau = (nu/f)/G;
    double dt = min(0.25*tau,t);
    double deps = 0.5*(Yf-Y0)/G;
    if( eps_el < 0 )
        deps = - deps;
      t    -= dt;
    eps_el -= deps*(1-exp(-dt/tau));
}

int VonMisesConstGV::Equilibrate(double V, double e, double *z)
{
    double eps_V  = elastic->Eps_el(V);
    double eps_el = eps_V - z[0];
    if( eps_el <= 0 )
    {
        if( eps_el < eps_y_compression )
            z[0] = eps_V - eps_y_compression;
    }
    else
    {
         if( eps_el > eps_y_expansion )
             z[0] = eps_V - eps_y_expansion;
    }
    return 0;
}
