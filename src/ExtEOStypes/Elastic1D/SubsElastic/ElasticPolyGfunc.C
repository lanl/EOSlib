#include "ElasticPolyG.h"

// EOS Functions

double ElasticPolyG::FD(double V, double e)
{
    return FD(V,e,Eps_el(V));
}


// elastic EOS functions

double ElasticPolyG::P(double V, double e, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return NaN;
    double e_hyd = e - PG->e(V,eps_el);
    return hydro->P(V,e_hyd)+PG->Pdev(V,eps_el)+PG->Ps(V,eps_el);
}

double ElasticPolyG::Phydro(double V, double e, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return NaN;
    double e_hyd = e - PG->e(V,eps_el);
    return hydro->P(V,e_hyd);
}

double ElasticPolyG::Pshear(double V, double e, double eps_el)
{
    if( PG == NULL )
        return NaN;
    return PG->Ps(V,eps_el);
}

double ElasticPolyG::Pdev(double V, double e, double eps_el)
{
    if( PG == NULL )
        return NaN;
    return PG->Pdev(V,eps_el);
}

double ElasticPolyG::T(double V, double e, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return NaN;
    double e_hyd = e - PG->e(V,eps_el);
    return hydro->T(V,e_hyd);
}

double ElasticPolyG::S(double V, double e, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return NaN;
    double e_hyd = e - PG->e(V,eps_el);
    return hydro->S(V,e_hyd);
}

double ElasticPolyG::CV(double V, double e, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return NaN;
    double e_hyd = e - PG->e(V,eps_el);
    return hydro->CV(V,e_hyd);
}

double ElasticPolyG::c2(double V, double e, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return NaN;
    double e_hyd = e - PG->e(V,eps_el);
    return hydro->c2(V,e_hyd)+PG->cT2(V,eps_el);
}

double ElasticPolyG::Gamma(double V, double e, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return NaN;
    double e_hyd = e - PG->e(V,eps_el);
    return hydro->Gamma(V,e_hyd);
}

double ElasticPolyG::FD(double V, double e, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return NaN;
    double e_hyd = e - PG->e(V,eps_el);
    double c2_hyd = hydro->c2(V,e_hyd);
    double FD_hyd = hydro->FD(V,e_hyd);
    return (c2_hyd*FD_hyd+0.5*V*V*V*PG->d2PdV2(V,eps_el))
           / (c2_hyd+PG->cT2(V,eps_el));
}

int ElasticPolyG::NotInDomain(double V, double e, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return 1;
    if( V <= 0 )
        return 1;
    double e_hyd = e - PG->e(V,eps_el);
    return hydro->NotInDomain(V,e_hyd); 
}
