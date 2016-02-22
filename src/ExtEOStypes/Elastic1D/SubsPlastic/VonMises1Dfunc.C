#include "VonMises1D.h"

double VonMises1D::f(double eps_el)
{// yield_function
    double Tve = elastic->T(V1,e1, eps_el);
    return isnan(Tve) ? NaN : yield_function(V1,Tve,eps_el);
}
//
double VonMises1D::yield_function(double V, double e, const double *z)
{
    double eps_el = elastic->Eps_el(V) - z[0];
    double Tve    = elastic->T(V,e, eps_el);
    return isnan(Tve) ? NaN : yield_function(V,Tve,eps_el);
}

double VonMises1D::yield_strength(double V, double e, const double *z)
{
    return Y0;
}

double VonMises1D::Nu(double eps_pl, double Y, double Yf)
{
    double f = 1;
    if( Y>0 && A2>0 )
        f += A2*pow((Yf/Y-1),n2);
    if( eps_y>0 && A1>0 )
        f += A1*pow(abs(eps_pl)/eps_y,n1);
    return nu/f;
}

int VonMises1D::Rate(double V, double e, const double *z, double *zdot)
{
    zdot[0] = 0.;
    double eps_el = elastic->Eps_el(V) - z[0];
    double Tve    = elastic->T(V,e,eps_el);
    if( isnan(Tve) )
        return -1;
    double Yf = yield_function(V,Tve,eps_el);
    double Y  = yield_strength(V,e,z);
    if( Yf > Y )
    {
        zdot[0] = 0.5*(Yf-Y)/Nu(z[0],Y,Yf);
        if( eps_el < 0 )
            zdot[0] = -zdot[0];
    }
    return 1; // ExtEOS::n
}

int VonMises1D::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0;
    double eps_el = elastic->Eps_el(V) - z[0];
    double Tve    = elastic->T(V,e,eps_el);
    if( isnan(Tve) )
        return -1;
    double Yf = yield_function(V,Tve,eps_el);
    double Y  = yield_strength(V,e,z);
    if( Yf <= Y )
        return 0;   // no constraint
    double G = (3./4.)*shear->dPdev_deps(V,Tve,eps_el);
    dt = Nu(z[0],Y,Yf)/G;  // e-fold of relaxation = (0.5*(Yf-Y)/G)/|Rate|
    return 2;       // Stiff Integrate available
}

int VonMises1D::Integrate(double V, double e, double *z, double dt)
{
    if( dt == 0 )
        return 0;
    if( dt < 0 )
        return 1;
    V1     = V;
    e1     = e;
    eps_V1 = elastic->Eps_el(V);
    Y1     = yield_strength(V,e,z);
    double eps_el = eps_V1 - z[0];
    if( frozen )
    {
        int count = 25;    
        while( count-- )
        {
            int status = Step(eps_el,dt);
            if( status < 0 )
                return 1;   // error
            else if( status==0 || dt <= 0 )
            {
                z[0] = eps_V1 - eps_el;
                return 0;
            }
        }
    }
    eps_el = inverse(Y1,0,2*eps_el);
    if( OneDFunction::Status() )
    {
        EOSerror->Log("VonMises1D::Integrate", __FILE__, __LINE__, this,
                      "inverse failed, V=%lf, e=%lf, z[0]=%lf\n",
                      V,e,z[0] );
        return 1;
    }
    z[0] = eps_V1 - eps_el;
    return 0;
}

int VonMises1D::Step(double &eps_el, double &t)
{
    double Tve = elastic->T(V1,e1,eps_el);
    if( isnan(Tve) )
        return -1;
    double Yf = yield_function(V1,Tve,eps_el);
    if( Yf <= Y1 )
        return 0;
    double G = 0.75*shear->dPdev_deps(V1,Tve,eps_el);
    double tau = Nu(eps_V1-eps_el,Y1,Yf)/G;
    double deps = 0.5*(Yf-Y1)/G;       // for Y = Yf
    if( eps_el < 0 )
        deps = -deps;
    double dt = min(0.25*tau,t);
      t    -= dt;
    eps_el -= deps*(1-exp(-dt/tau));
    return 1;
}

int VonMises1D::Equilibrate(double V, double e, double *z)
{
    double eps_V = elastic->Eps_el(V);
    double eps0   = eps_V - z[0];
    double Tve = elastic->T(V,e,eps0);
    if( isnan(Tve) )
        return -1;  // error

    double Yz0 = yield_function(V,Tve,eps0);
    double Y   = yield_strength(V,e,z);
    if( Yz0 < Y + 10*P_vac )
        return 0;

    double sign_s = (eps0>0) ? 1. : -1.;
    double eps1 = eps0;
    int count = 10;
    while( count-- )
    {
        double G = 0.75*shear->dPdev_deps(V,Tve,eps1);
        eps1 -= sign_s*0.5*(Yz0-Y)/G;
        Tve = elastic->T(V,e,eps1);
        double Yz1 = yield_function(V,Tve,eps1);
        if( abs(Yz1-Y) < 1e-6*Y + 10*P_vac )
        {
            z[0] = eps_V - eps1;
            return 0;
        }
        eps0 = eps1;
        Yz0 = Yz1;
    }
    V1 = V;
    e1 = e;
    double eps = inverse(Y,0,sign_s*2*eps_y);
    if( OneDFunction::Status() )
    {
        EOSerror->Log("VonMises1D::Equilibrate", __FILE__, __LINE__, this,
                      "failed, V=%lf, e=%lf, z[0]=%lf\n", V,e,z[0] );
        return 1;
    }
    z[0] = eps_V-eps;
    return 0;
}
