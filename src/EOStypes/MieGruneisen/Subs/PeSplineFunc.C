#include "PeSpline.h"

double PeSpline::P(double V, double e)
{
    return Pref(V) + GoV(V)*(e-eref(V));
}
double PeSpline::e_PV(double p, double V)
{
    return eref(V) + (p-Pref(V))/GoV(V);
}

double PeSpline::T(double V, double e)
{
    return Tref(V) + (e-eref(V))/Cv;
}

double PeSpline::S(double V, double e)
{
    return S0 + Cv*(log(T(V,e)/T0)+intGoV(V));
}

double PeSpline::Gamma(double V, double)
{
    return GoV(V)*V;
}

double PeSpline::CV(double, double)
{
    return Cv;
}

double PeSpline::c2(double V, double e)
{
    return V*V*(rhoc2ref(V)+(sqr(GoV(V))-dGoV(V))*(e-eref(V)));
}

double PeSpline::FD(double V, double e)
{
    double de    = e - eref(V);
    double gov   = GoV(V);
    double dgov  = dGoV(V);
    double gg    = sqr(gov)-dgov;
    double rhoc2 = rhoc2ref(V)+gg*de;
    double drhoc2 = drhoc2ref(V) - gg*(Pref(V)+deref(V))
                                 + (gov*(2*dgov-gg)-d2GoV(V))*de;
    return -0.5*V*drhoc2/rhoc2; 
}

int PeSpline::NotInDomain(double V, double e)
{
    return V<Vmin || Vmax<V;
}
