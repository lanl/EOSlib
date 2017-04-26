#include "JWL.h"

double JWL::P(double V, double e)
{
    if( V<=0. )
        return NaN;
    
    double v = V/V0;
    return A*(1.-omega/(R1*v))*exp(-R1*v) + B*(1.-omega/(R2*v))*exp(-R2*v)
           + omega*(e+de)/V;
}
double JWL::e_PV(double p, double V)
{
    if( V<=0. )
        return NaN;
    
    double v = V/V0;
    double pref = A*(1.-omega/(R1*v))*exp(-R1*v)+B*(1.-omega/(R2*v))*exp(-R2*v);
    return de + V*(p-pref)/omega;
}

double JWL::c2(double V, double e)
{
    if( V <= 0. )
        return NaN;
    double v = V/V0;
    double ww = omega*(1.+omega);
    double csq = A*V0/R1*(sqr(v*R1)-ww)*exp(-R1*v)
               + B*V0/R2*(sqr(v*R2)-ww)*exp(-R2*v)
               + ww*(e+de);
    return csq > 0. ? csq : 0.0;
}

double JWL::FD(double V, double e)
{
// on isentrope
// P_s(V) = A*exp(-R1*V/V0) + B*exp(-R2*V/V0) + C*(V0/V)^(omega+1)
// where C characterizes the isentrope.
// For isentrope through (V1,e1)
// C=omega*(V1/V0)^omega*(e1/V0-(A/R1)*exp(-R1*V1/V0)-(B/R2)*exp(-R2*V1/V0))
    if( V <= 0. )
        return NaN;
    double v = V/V0;
    double Ae = A*exp(-R1*v);
    double Be = B*exp(-R2*v);
    double Cs = omega*((e+de)/V0 -Ae/R1 - Be/R2)/v;
    double P1 = Ae*R1*v + Be*R2*v + Cs*(omega+1);
    double P2 = Ae*sqr(R1*v) + Be*sqr(R2*v) + Cs*(omega+1)*(omega+2);
    
    return 0.5*P2/P1;
}

double JWL::Gamma(double V, double e)
{
    return omega;
}

//


int JWL::NotInDomain(double V, double e)
{
    return V < 0. || P(V,e) < P_vac || c2(V,e) <= 0.;
}

// Thermal properties

double JWL::T(double V, double e)
{
    return Tref(V) + (e-eref(V))/Cv;
}

double JWL::S(double V, double e)
{
    return Cv*(log(T(V,e)/Tcj)+omega*log(V/Vcj)) + Scj;
}

double JWL::CV(double, double)
{
    return Cv;
}

// Reference curve (CJ isentrope)

double JWL::Pref(double V)
{
    return A*exp(-R1*V/V0)+B*exp(-R2*V/V0)+C*pow(V0/V,omega+1);
}

double JWL::eref(double V)
{
    return V0*(A/R1*exp(-R1*V/V0)+B/R2*exp(-R2*V/V0)+C/omega*pow(V0/V,omega))
           -de;
}

double JWL::Tref(double V)
{
    return Tcj*pow(Vcj/V,omega);
}
