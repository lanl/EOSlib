#include "ConstGV.h"

double ConstGV::F(double V, double, double eps_el)
{
    return e(V,eps_el); 
}

double ConstGV::S(double, double, double)
{
    return 0.0;
}

double ConstGV::e(double V, double, double eps_el)
{
    return e(V,eps_el); 
}

double ConstGV::Ps(double, double, double)
{
    return 0.0;
}

double ConstGV::Pdev(double V, double, double eps_el)
{
    return Pdev(V,eps_el);
}

double  ConstGV::dPdev_deps(double V, double, double eps_el)
{
    return dPdev_deps(V,eps_el);
}

double ConstGV::CV(double V, double, double eps_el)
{
    return 0.0;
}

double ConstGV::dPdT(double, double, double)
{
    return 0.0;
}

double ConstGV::cT2(double V, double, double eps_el)
{
    return cT2(V,eps_el);
}

double ConstGV::cs2(double V, double, double eps_el)
{
    return cs2(V,eps_el);
}
