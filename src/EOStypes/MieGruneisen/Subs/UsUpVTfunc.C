#include "UsUp_VT.h"

double UsUp_VT::P(double V, double T)
{
    return Pref(V) + GoV(V)*Cv*(T-Tref(V));
}

double UsUp_VT::e(double V, double T)
{
    return eref(V) + (T-Tref(V))*Cv;
}

double UsUp_VT::S(double V, double T)
{
    return S0 + Cv*(log(T/T0)+intGoV(V));
}

double UsUp_VT::P_V(double V, double T)
{
    double dPdV = dPref(V);
    double gov = GoV(V);
    //return sqr(gov)*Cv*T
    //  + dPdV-0.5*(dPdV*(V0-V)+(Pref(V)-P0))*gov
    //  +(dGoV(V)-sqr(gov))*Cv*(T-Tref(V));
    //
    // P(V,e) = Pref(V) + [Gamma(V)/V]*[e-eref(V)]
    //   e-eref(V) = Cv*[T-Tref(V)]
    //   eref(V) = e0 + 0.5*(Pref(V)+P0)*(V0-V)
    //  dTrefdV = -gov*Tref + (Pref+derefdV)/Cv
    double TrefV = Tref(V);
    return dPdV + gov*( gov*Cv*TrefV -0.5*((Pref(V)-P0)+dPdV*(V0-V)) )
           + dGoV(V)*Cv*(T-TrefV);
}

double UsUp_VT::P_T(double V, double T)
{
    return GoV(V)*Cv;
}

double UsUp_VT::CV(double V, double T)
{
    return Cv;
}

double UsUp_VT::Gamma(double V, double T)
{
    return GoV(V)*V;
}

double UsUp_VT::c2(double V, double T)
{
    double dPdV = dPref(V);
    double gov = GoV(V);
    return -V*V*( dPdV - 0.5*(dPdV*(V0-V)+(Pref(V)-P0))*gov
                       + (dGoV(V)-sqr(gov))*Cv*(T-Tref(V)) );
}

int UsUp_VT::NotInDomain(double V, double T)
{
    return T < 0 || V/V0 <= 1 - 1/s;
}
