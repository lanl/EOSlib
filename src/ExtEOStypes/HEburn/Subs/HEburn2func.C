#include <string.h>

#include "HEburn2.h"
//
// ToDo: change defines to input parameters
#define LAMBDA_MIN 0.10
#define LAMBDA_MAX 0.90
//
int HEburn2::set_z(const char *var, double value, double *z)
{
    int status = ExtEOS::set_z(var,value,z);
    if( status<=0 ) return status;
    return rate ? rate->set_z(var,value,z) : -1;
}
int HEburn2::get_z(double *z, const char *var, double &value)
{
    int status = ExtEOS::get_z(z,var,value);
    if (status<=0 ) return status;
    return rate ? rate->get_z(z,var,value) : -1;
}

int HEburn2::var(const char *name, double V, double e, const double *z,
                         double &value)
{
    int status = ExtEOS::ExtEOS::var(name,V,e,z,value);
    return (status<=0) ? status : rate->var(name,V,e,z,value);
}

double *HEburn2::z_f(double V, double e)
{
    double lambda  = 1.0;
    double lambda2 = 1.0;;
    if( frozen == 1 )
    {
        (void) lambda_ref(lambda, lambda2);
    }
    rate->set_lambda(lambda,lambda2,ztmp);
    return ztmp;        
}
//
int HEburn2::ddz(int i, double V,   double e,  const double *z,
                            double &dP, double &dT)
{
    return 1;   // not implemented
                // HE->dPdlambda and HE-dTdlambda are available
}

double HEburn2::P(double V, double e, const double *z)
{
    if( set_eos(z) )
        return NaN;
    double p = HE->P(V,e);
    if( std::isnan(p) )
    {
        if( HE->lambda2 < LAMBDA_MIN )
            return HE->eos1->P(V,e);
        else if( LAMBDA_MAX < HE->lambda2 )
            return HE->eos2->P(V,e+HE->de);
    }
    return p;
}
double HEburn2::e_PV(double p, double V, const double *z)
{
    if( set_eos(z) )
        return NaN;
    return HE->e_PV(p,V);
}

double HEburn2::T(double V, double e, const double *z)
{
    if( set_eos(z) )
        return NaN;
    double Tz = HE->T(V,e);
    if( std::isnan(Tz) )
    {
        if( HE->lambda2 < LAMBDA_MIN )
            return HE->eos1->T(V,e);
        else if( LAMBDA_MAX < HE->lambda2 )
            return HE->eos2->T(V,e+HE->de);
    }
    return Tz;
}

double HEburn2::S(double V, double e, const double *z)
{
    if( set_eos(z) )
        return NaN;
    double s = HE->S(V,e);
    if( std::isnan(s) )
    {
        if( HE->lambda2 < LAMBDA_MIN )
            return HE->eos1->S(V,e);
        else if( LAMBDA_MAX < HE->lambda2 )
            return HE->eos2->S(V,e+HE->de);
    }
    return s;
}

double HEburn2::c2(double V, double e, const double *z)
{
    if( set_eos(z) )
        return NaN;
    double c_2 = HE->c2(V,e);
    if( std::isnan(c_2) || c_2 <= 0.0 )
    {
        if( HE->lambda2 < LAMBDA_MIN )
            return HE->eos1->c2(V,e);
        else if( LAMBDA_MAX < HE->lambda2 )
            return HE->eos2->c2(V,e+HE->de);
    }
    return c_2;
}

double HEburn2::Gamma(double V, double e, const double *z)
{
    if( set_eos(z) )
        return NaN;
    double G = HE->Gamma(V,e);
    if( std::isnan(G) )
    {
        if( HE->lambda2 < LAMBDA_MIN )
            return HE->eos1->Gamma(V,e);
        else if( LAMBDA_MAX < HE->lambda2 )
            return HE->eos2->Gamma(V,e+HE->de);
    }
    return G;
}

double HEburn2::CV(double V, double e, const double *z)
{
    return set_eos(z) ? NaN : HE->CV(V,e);
}

double HEburn2::CP(double V, double e, const double *z)
{
    return set_eos(z) ? NaN : HE->CP(V,e);
}

double HEburn2::KT(double V, double e, const double *z)
{
    return set_eos(z) ? NaN : HE->KT(V,e);
}

double HEburn2::beta(double V, double e, const double *z)
{
    return set_eos(z) ? NaN : HE->beta(V,e);
}

double HEburn2::FD(double V, double e, const double *z)
{
    return set_eos(z) ? NaN : HE->FD(V,e);
}

int HEburn2::NotInDomain(double V, double e, const double *z)
{
    return set_eos(z) ? 1 : HE->NotInDomain(V,e);
}
