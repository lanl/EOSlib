#include "Elastic.h"

double ElasticEOS::P(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->P(V, Tve, z);   
}

double ElasticEOS::T(double V, double e, const double *z)
{
    if( EOSstatus != EOSlib::good )
        return NaN;

    double Tmax = hydro->T(V, e);
    double eshear = elastic->eshear(V,Tmax, z[0]);
    if( abs(eshear) <= 1e-6*abs(e) )
        return Tmax;
    double de_max = eshear;                                     // emax - e
    double Tmin = hydro->T(V, e-eshear);
    double de_min = eshear - elastic->eshear(V, Tmin, z[0]);    // e - emin
 // Assume eshear is small and interpolate
 // otherwise would need to iterate
    return (de_max*Tmin + de_min*Tmax)/(de_max+de_min);
}

double ElasticEOS::S(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->S(V, Tve, z);   
}

double ElasticEOS::c2(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->c2(V, Tve, z);   
}

double ElasticEOS::Gamma(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->Gamma(V, Tve, z);   
}

double ElasticEOS::CV(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->CV(V, Tve, z);   
}

double ElasticEOS::CP(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->CP(V, Tve, z);   
}

double ElasticEOS::KT(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->KT(V, Tve, z);   
}

double ElasticEOS::beta(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? NaN : elastic->beta(V, Tve, z);   
}

int ElasticEOS::NotInDomain(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? 1 : elastic->NotInDomain(V,Tve, z);
}    

int ElasticEOS::Rate(double V, double e, const double *z, double *zdot)
{
    double Tve = T(V,e, z);
    return std::isnan(Tve) ? 1 : elastic->Rate(V,Tve, z, zdot);
}
