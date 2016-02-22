#include "PolyG.h"

double PolyG::F(double V, double T, double eps_el)
{
    return e(V,eps_el); 
}

double PolyG::S(double, double, double)
{
    return 0.0;
}

double PolyG::e(double V, double, double eps_el)
{
    return e(V,eps_el); 
}

double PolyG::Ps(double V, double, double eps_el)
{
    return Ps(V,eps_el); 
}

double PolyG::Pdev(double V, double, double eps_el)
{
    return Pdev(V,eps_el); 
}

double  PolyG::dPdev_deps(double V, double, double eps_el)
{
    return dPdev_deps(V,eps_el); 
}

double PolyG::CV(double V, double, double eps_el)
{
    return 0.0;
}

double PolyG::dPdT(double, double, double)
{
    return 0.0;
}

double PolyG::cT2(double V, double, double eps_el)
{
    return cT2(V,eps_el); 
}

double PolyG::cs2(double V, double, double eps_el)
{
    return cs2(V,eps_el); 
}

//  d2PdV2 = (d/dV+(1/V)*d/deps_el)^2 (Ps+Pdev)
//         = -(d/dV+(1/V)*d/deps_el) (cT2/V^2)
double PolyG::d2PdV2(double V, double eps_el)
{
    double x = exp((2./3.)*eps_el);
    double eta = 1.-V/V0;
    //cT2/V^2 =  ( (2./9.)*(7.*x*x -1./x)*G.f(eta)
    //       -((7./3.)*x*x +(2./3.)/x-3.)*(V/V0)*G.df(eta)
    //       +0.5*(x*x +2./x-3.)*sqr(V/V0)*G.d2f(eta) )/V;
    double g  = G.f(eta);
    double g1 = G.df(eta);
    double g2 = G.d2f(eta);
    double g3 = G.d3f(eta);
    double sum;
    sum = -(2./9.)*(7.*x*x -1./x)*g/V
           +0.5*(x*x +2./x-3.)*V/sqr(V0)*g2;
    sum -= ( (2./9.)*(7.*x*x -1./x)*g1
           -((7./3.)*x*x +(2./3.)/x-3.)*(V/V0)*g2
           +0.5*(x*x +2./x-3.)*sqr(V/V0)*g3 )/V0;
    sum += (2./3.)*( (2./9.)*(14.*x*x +1./x)*g
           -((14./3.)*x*x -(2./3.)/x)*(V/V0)*g1
           +(x*x -1./x)*sqr(V/V0)*g2 )/V;
    return -sum/V;
}
