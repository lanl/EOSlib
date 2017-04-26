#include "DavisProducts.h"


double DavisProducts::Pref(double V)
{
    double v = V/Vc;
    double vn = pow(v,n);
    double vk = pow(v,k+a);
    double va = pow(0.5*(vn+1./vn),a/n);
    return pc*va/vk * (k-1.+fref(vn))/(k-1.+a);
}
double DavisProducts::dPref(double V)
{
    double v = V/Vc;
    double vn = pow(v,n);
    double vk = pow(v,k+a);
    double va = pow(0.5*(vn+1./vn),a/n);
    double f = fref(vn);
    double p = pc*va/vk * (k-1.+f)/(k-1.+a);
    double df = n*sqr(f*vn)/a;
    return (a*(vn*vn-1.)/(vn*vn+1.)-(k+a)- df/(k-1.+f))*p/V;
}

double DavisProducts::eref(double V)
{
    double v = V/Vc;
    double vn = pow(v,n);
    double vk = pow(v,k-1.+a);
    double va = pow(0.5*(vn+1./vn),a/n);
    return ec*va/vk-e0;
}

double DavisProducts::Tref(double V)
{
    double v = V/Vc;
    double vn = pow(v,n);
    double vk = pow(v,k-1.+a*(1.-b));
    double va = pow(0.5*(vn+1./vn),a/n*(1.-b));
    return Tc*va/vk;
}

double DavisProducts::Gamma(double V, double)
{
    if( V<=0. )
        return NaN;
    double vn = pow(V/Vc,n);
    return k-1. +(1.-b)*fref(vn);
}

double DavisProducts::P(double V, double e)
{
    if( V<=0. )
        return NaN;
    double vn = pow(V/Vc,n);
    double G = k-1. +(1.-b)*fref(vn);
    return Pref(V) + G/V *(e-eref(V));
}
double DavisProducts::e_PV(double p, double V)
{
    if( V<=0. )
        return NaN;
    double vn = pow(V/Vc,n);
    double G = k-1. +(1.-b)*fref(vn);
    return eref(V) + V *(p-Pref(V))/G;
}

double DavisProducts::T(double V, double e)
{
    return Tref(V) + (e-eref(V))/Cv;
}

double DavisProducts::S(double V, double e)
{
    if( V<=0. )
        return NaN;
    double v = V/Vc;
    double vn = pow(v,n);

    double vk = pow(v,k-1.+a*(1.-b));
    double va = pow(0.5*(vn+1./vn),a/n*(1.-b));
    return Cv*log( (T(V,e)/T0)*(vk/va) ) + S0;
}

double DavisProducts::CV(double, double)
{
    return Cv;
}

int DavisProducts::NotInDomain(double V, double e)
{
    return V < 0. || P(V,e) < 0. || c2(V,e) <= 0.;
}

//

double DavisProducts::c2(double V, double e)
{
    if( V <= 0. )
        return NaN;
    double v = V/Vc;
    double vn = pow(v,n);
    double fn = fref(vn);
    double GoV = (k-1. +(1.-b)*fn)/V;
    double dGoV = -GoV/V -(1.-b)*n*sqr(fn*vn/V)/a;
    return V*V*(-dPref(V)+(sqr(GoV)-dGoV)*(e-eref(V)));
}
