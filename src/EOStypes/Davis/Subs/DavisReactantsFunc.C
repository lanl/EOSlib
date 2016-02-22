#include "DavisReactants.h"


double DavisReactants::Pref(double V)
{
    double y = 1.-V/V0;
    double yb = 4.*B*y;
    return P0 + ps*(yb*(1.+yb/2.*(1.+yb/3.*(1.+C*yb/4.)))+sqr(y*sqr(V0/V)));    
}
double DavisReactants::dPref(double V)
{
    double y = 1.-V/V0;
    double yb = 4.*B*y;
    return -ps*(4.*B/V0*(1.+yb*(1.+yb/2.*(1.+C*yb/3.)))
                +y*sqr(sqr(V0/V))*(2./V0 + 4.*y/V));  
}

double DavisReactants::eref(double V)
{
    double y = 1. - V/V0;
    double yb = 4.*B*y;
    double v = V0/V;
    return e0 + P0*(V0-V)
              + ps*V0*(0.5*y*yb*(1.+yb/3.*(1.+yb/4.*(1+C*yb/5.)))
                       + v*(1.-v*(1.-v/3.)) - 1./3.);
}

double DavisReactants::Tref(double V)
{
    double v = V/V0;
    return T0*exp(-Z*(1.-v))*pow(v,-(G0+Z));
}

double DavisReactants::Gamma(double V, double)
{
    if( V<=0. )
        return NaN;
    return G0 + Z*(1.-V/V0);
}

double DavisReactants::P(double V, double e)
{
    if( V<=0. )
        return NaN;
    double G = G0 + Z*(1.-V/V0);
    return Pref(V) + G/V *(e-eref(V));
}
double DavisReactants::e_PV(double p, double V)
{
    if( V<=0. )
        return NaN;
    double G = G0 + Z*(1.-V/V0);
    return eref(V) + V *(p-Pref(V))/G;
}

double DavisReactants::T(double V, double e)
{
    if( V<=0. )
        return NaN;    
    double Ts = Tref(V);
    return Ts*pow(1.+(1.+alpha)*(e-eref(V))/(Cv*Ts),1./(1.+alpha));
}

double DavisReactants::S(double V, double e)
{
    if( V<=0. )
        return NaN;
    double Ts = Tref(V);
    double es = eref(V);
    return S0+Cv*(pow(1.+(1.+alpha)*(e-es)/(Cv*Ts),alpha/(1.+alpha))-1.)/alpha;
}

double DavisReactants::CV(double V, double e)
{
    return Cv+alpha*(S(V,e)-S0);
}

int DavisReactants::NotInDomain(double V, double e)
{
    return V < 0. || P(V,e) < 0. || c2(V,e) <= 0.;
}

//

double DavisReactants::c2(double V, double e)
{
    if( V <= 0 )
        return NaN;
    double GoV = (G0 + Z*(1.-V/V0))/V;
    double dGoV = -(G0+Z)/sqr(V);
    return V*V*(-dPref(V)+(sqr(GoV)-dGoV)*(e-eref(V)));
}
