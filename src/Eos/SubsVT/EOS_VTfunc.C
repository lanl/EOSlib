#include "EOS_VT.h"
#include <LocalMath.h>

double EOS_VT::F(double V, double T)
{
    return e(V,T) - T*S(V,T);
}

int EOS_VT::NotInDomain(double V, double T)
{
    if( V <= 0 || T < 0 )
        return 1;
    double csq = cT2(V,T);
    return (isnan(csq) || csq <= 0.0) ? 1 : 0;
}

double EOS_VT::CP(double V, double T)
{ // Specific heat at const pressure
    return CV(V,T) + V*T/KT(V,T)*sqr(P_T(V,T));
}

double EOS_VT::KS(double V, double T)
{ // isentropic bulk modulus
    return -V*P_V(V,T) + T*V/CV(V,T)*sqr(P_T(V,T));
}

double EOS_VT::KT(double V, double T)
{ // isothermal bulk modulus
    return -V*P_V(V,T);
}

double EOS_VT::Gamma(double V, double T)
{ // Gruneisen coefficient
    return V*P_T(V,T)/CV(V,T);
}

double EOS_VT::beta(double V, double T)
{ // coefficient of thermal expansion
    return P_T(V,T)/KT(V,T);
}

double EOS_VT::c2(double V, double T)
{ // isentropic sound speed squared
    return V*KS(V,T);
}

double EOS_VT::cT2(double V, double T)
{ // isothermal sound speed squared
    return V*KT(V,T);
}
