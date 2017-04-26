#include "UsUpSpline_VT.h"

double UsUpSpline_VT::P(double V, double T)
{
    return Pref(V) + GoV(V)*Cv*(T-Tref(V));
}

double UsUpSpline_VT::e(double V, double T)
{
    return eref(V) + (T-Tref(V))*Cv;
}

double UsUpSpline_VT::S(double V, double T)
{
    return S0 + Cv*(log(T/T0)+intGoV(V));
}

double UsUpSpline_VT::P_V(double V, double T)
{
    double dPdV = dPref(V);
    double gov = GoV(V);
    return sqr(gov)*Cv*T +
      dPdV-0.5*(dPdV*(V0-V)+(Pref(V)-P0))*gov+(dGoV(V)-sqr(gov))*Cv*(T-Tref(V));
}

double UsUpSpline_VT::P_T(double V, double T)
{
    return GoV(V)*Cv;
}

double UsUpSpline_VT::CV(double V, double T)
{
    return Cv;
}

double UsUpSpline_VT::Gamma(double V, double T)
{
    return GoV(V)*V;
}

double UsUpSpline_VT::c2(double V, double T)
{
    double dPdV = dPref(V);
    double gov = GoV(V);
    return -V*V*( dPdV - 0.5*(dPdV*(V0-V)+(Pref(V)-P0))*gov
                       + (dGoV(V)-sqr(gov))*Cv*(T-Tref(V)) );
}

int UsUpSpline_VT::NotInDomain(double V, double T)
{
    return T < 0 || V <= V1;
}
