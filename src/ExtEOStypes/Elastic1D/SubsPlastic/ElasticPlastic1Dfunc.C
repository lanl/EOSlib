#include "ElasticPlastic1D.h"

double ElasticPlastic1D::P(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->P(V,e,eps);
}

double ElasticPlastic1D::Phydro(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->Phydro(V,e,eps);
}

double ElasticPlastic1D::Pshear(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->Pshear(V,e,eps);
}

double ElasticPlastic1D::Pdev(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->Pdev(V,e,eps);
}

double ElasticPlastic1D::T(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->T(V,e,eps);
}

double ElasticPlastic1D::S(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->S(V,e,eps);
}

double ElasticPlastic1D::c2(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->c2(V,e,eps);
}

double ElasticPlastic1D::Gamma(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->Gamma(V,e,eps);
}

double ElasticPlastic1D::CV(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->CV(V,e,eps);
}

int ElasticPlastic1D::NotInDomain(double V, double e, const double *z)
{
    if( elastic == NULL )
        return 1;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->NotInDomain(V,e,eps);
}

//

double ElasticPlastic1D::CP(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->CP(V,e,eps);
}

double ElasticPlastic1D::beta(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->beta(V,e,eps);
}

double ElasticPlastic1D::KT(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->KT(V,e,eps);
}

double ElasticPlastic1D::FD(double V, double e, const double *z)
{
    if( elastic == NULL )
        return NaN;
    double eps = elastic->Eps_el(V) - z[0];
    return elastic->FD(V,e,eps);
}
