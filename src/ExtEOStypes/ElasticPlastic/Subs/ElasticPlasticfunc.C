#include "ElasticPlastic.h"

double ElasticPlastic::P(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->P(V, Tve, z_el);   
}

double ElasticPlastic::T(double V, double e, const double *z)
{
    if( EOSstatus != EOSlib::good )
        return NaN;
    z_el[0] = elastic->eps_el(V) - z[0];

    double Tmax = hydro->T(V, e);
    double eshear = elastic->eshear(V,Tmax, z_el[0]);
    if( std::abs(eshear) <= 1e-6*std::abs(e) )
        return Tmax;
    double de_max = eshear;                                     // emax - e
    double Tmin = hydro->T(V, e-eshear);
    double de_min = eshear - elastic->eshear(V, Tmin, z_el[0]); // e - emin
 // Assume eshear is small and interpolate
 // otherwise would need to iterate
    return (de_max*Tmin + de_min*Tmax)/(de_max+de_min);
}

double ElasticPlastic::S(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->S(V, Tve, z_el);   
}

double ElasticPlastic::c2(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->c2(V, Tve, z_el);   
}

double ElasticPlastic::Gamma(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->Gamma(V, Tve, z_el);   
}

double ElasticPlastic::CV(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->CV(V, Tve, z_el);   
}

double ElasticPlastic::CP(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->CP(V, Tve, z_el);   
}

double ElasticPlastic::KT(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->KT(V, Tve, z_el);   
}

double ElasticPlastic::beta(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->beta(V, Tve, z_el);   
}

int ElasticPlastic::NotInDomain(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? 1 : elastic->NotInDomain(V,Tve, z_el);
}
