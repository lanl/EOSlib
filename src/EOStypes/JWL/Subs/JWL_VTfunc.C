#include "JWL_VT.h"

double JWL_VT::P(double V, double T)
{
    if( V<=0. )
        return NaN;    
    double v = V/V0;
    return A*(1. - omega/(R1*v))*exp(-R1*v) + B*(1. - omega/(R2*v))*exp(-R2*v)
           + omega*(e(V,T)+de)/V;
}

double JWL_VT::e(double V, double T)
{
    return eref(V) +Cv*(T-Tref(V));
}

double JWL_VT::S(double V, double T)
{
    return Cv*(log(T/Tcj)+omega*log(V/Vcj)) + Scj;
}

double JWL_VT::P_V(double V, double T)
{
    return sqr(omega/V)*Cv*T - c2(V,T)/sqr(V);
}

double JWL_VT::P_T(double V, double T)
{
    return omega*Cv/V;
}

double JWL_VT::CV(double V, double T)
{
    return Cv;
}

double JWL_VT::Gamma(double V, double T)
{
    return omega;
}

double JWL_VT::c2(double V, double T)
{
    if( V <= 0 )
        return NaN;
    double v = V/V0;
    double ww = omega*(1+omega);
    double csq = A*V0/R1*(sqr(v*R1)-ww)*exp(-R1*v)
               + B*V0/R2*(sqr(v*R2)-ww)*exp(-R2*v)
               + ww*(e(V,T)+de);
    return csq > 0. ? csq : 0.0;
}

int JWL_VT::NotInDomain(double V, double T)
{
    return T < 0. || P_V(V,T) > 0.;
}

// reference curve is isentrope thru CJ state

double JWL_VT::Pref(double V)
{
    return A*exp(-R1*V/V0)+B*exp(-R2*V/V0)+C*pow(V0/V,omega+1);
}

double JWL_VT::eref(double V)
{
    return V0*(A/R1*exp(-R1*V/V0)+B/R2*exp(-R2*V/V0)+C/omega*pow(V0/V,omega))
           -de;
}

double JWL_VT::Tref(double V)
{
    return Tcj*pow(Vcj/V,omega);
}
