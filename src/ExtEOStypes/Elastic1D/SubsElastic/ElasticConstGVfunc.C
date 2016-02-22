#include "ElasticConstGV.h"

// EOS Functions

double ElasticConstGV::FD(double V, double e)
{
    return FD(V,e,Eps_el(V));
}


// elastic EOS functions

double ElasticConstGV::P(double V, double e, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return NaN;
    double e_hyd = e - GV->e(V,eps_el);
    return hydro->P(V,e_hyd)+GV->Pdev(V,eps_el);
}

double ElasticConstGV::Phydro(double V, double e, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return NaN;
    double e_hyd = e - GV->e(V,eps_el);
    return hydro->P(V,e_hyd);
}

double ElasticConstGV::Pshear(double V, double e, double eps_el)
{
    return 0.0;
}

double ElasticConstGV::Pdev(double V, double e, double eps_el)
{
    if( GV == NULL )
        return NaN;
    return GV->Pdev(V,eps_el);
}

double ElasticConstGV::T(double V, double e, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return NaN;
    double e_hyd = e - GV->e(V,eps_el);
    return hydro->T(V,e_hyd);
}

double ElasticConstGV::S(double V, double e, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return NaN;
    double e_hyd = e - GV->e(V,eps_el);
    return hydro->S(V,e_hyd);
}

double ElasticConstGV::CV(double V, double e, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return NaN;
    double e_hyd = e - GV->e(V,eps_el);
    return hydro->CV(V,e_hyd);
}

double ElasticConstGV::c2(double V, double e, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return NaN;
    double e_hyd = e - GV->e(V,eps_el);
    return hydro->c2(V,e_hyd)+GV->cT2(V,eps_el);
}

double ElasticConstGV::Gamma(double V, double e, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return NaN;
    double e_hyd = e - GV->e(V,eps_el);
    return hydro->Gamma(V,e_hyd);
}

double ElasticConstGV::FD(double V, double e, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return NaN;
    double e_hyd = e - GV->e(V,eps_el);
    double c2_hyd = hydro->c2(V,e_hyd);
    double FD_hyd = hydro->FD(V,e_hyd);
    return (c2_hyd*FD_hyd+0.5*V*V*V*GV->d2PdV2(V,eps_el))
           / (c2_hyd+GV->cT2(V,eps_el));
}

int ElasticConstGV::NotInDomain(double V, double e, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return 1;
    if( V <= 0 )
        return 1;
    double e_hyd = e - GV->e(V,eps_el);
    return hydro->NotInDomain(V,e_hyd); 
}
