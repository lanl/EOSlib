#include "PeSpline_VT.h"

double PeSpline_VT::P(double V, double T)
{
    return Pref(V) + GoV(V)*Cv*(T-Tref(V));
}

double PeSpline_VT::e(double V, double T)
{
    return eref(V) + (T-Tref(V))*Cv;
}

double PeSpline_VT::S(double V, double T)
{
    return S0 + Cv*(log(T/T0)+intGoV(V));
}

double PeSpline_VT::P_V(double V, double T)
{
    double dPdV = dPref(V);
    double gov = GoV(V);
    return sqr(gov)*Cv*T +
      dPdV-0.5*(dPdV*(V0-V)+(Pref(V)-P0))*gov+(dGoV(V)-sqr(gov))*Cv*(T-Tref(V));
}

double PeSpline_VT::P_T(double V, double T)
{
    return GoV(V)*Cv;
}

double PeSpline_VT::CV(double V, double T)
{
    return Cv;
}

double PeSpline_VT::Gamma(double V, double T)
{
    return GoV(V)*V;
}

double PeSpline_VT::c2(double V, double T)
{
    double dPdV = dPref(V);
    double gov = GoV(V);
    return -V*V*( dPdV - 0.5*(dPdV*(V0-V)+(Pref(V)-P0))*gov
                       + (dGoV(V)-sqr(gov))*Cv*(T-Tref(V)) );
}

int PeSpline_VT::NotInDomain(double V, double T)
{
    return T < 0 || V < Vmin || Vmax < V;
}
