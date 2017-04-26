#include "UsUp.h"

double UsUp::P(double V, double e)
{
    return Pref(V) + GoV(V)*(e-eref(V));
}
double UsUp::e_PV(double p, double V)
{
    return eref(V) + (p-Pref(V))/GoV(V);
}

double UsUp::T(double V, double e)
{
    return Tref(V) + (e-eref(V))/Cv;
}

double UsUp::S(double V, double e)
{
    return S0 + Cv*(log(T(V,e)/T0)+intGoV(V));
}

double UsUp::Gamma(double V, double)
{
    return GoV(V)*V;
}

double UsUp::CV(double, double)
{
    return Cv;
}

double UsUp::c2(double V, double e)
{
/*************
    double dPdV = dPref(V);
    double gov = GoV(V);
    return -V*V*( dPdV - 0.5*(dPdV*(V0-V)+(Pref(V)-P0))*gov
                       + (dGoV(V)-sqr(gov))*(e-eref(V)) );
***************/
    return c2ref(V)+V*V*(sqr(GoV(V))-dGoV(V))*(e-eref(V));
}

double UsUp::FD(double V, double e)
{
    double p_ref = Pref(V);
    double dPdV  = dPref(V);
    double de    = e - eref(V);
    double gov   = GoV(V);
    double dgov  = dGoV(V);
    double rhoc2 = -(dPdV-0.5*(dPdV*(V0-V)+(p_ref-P0))*gov+(dgov-sqr(gov))*de);
    double d2P   = d2Pref(V) + de*d2GoV(V) -2*dgov*deref(V)-gov*d2eref(V)
                   +gov*rhoc2 -2*(p_ref+gov*de)*dgov;
    return 0.5*V*d2P/rhoc2;
}

int UsUp::NotInDomain(double V, double e)
{
    return V/V0 <= 1 - 1/s || Vmax < V;
}
