#include "HEburn.h"

// ToDo: change defines to input parameters
#define LAMBDA_MIN 0.10
#define LAMBDA_MAX 0.90

double *HEburn::z_f(double V, double e)
{
    ztmp[ilambda] = frozen ? (*z_ref)(ilambda) : 1;
    return ztmp;        
}

int HEburn::set_lambda(double lambda)
{
    if( HE == NULL )
        return 1;
    if( HE->lambda2 != lambda )
    {
        HE->lambda1 = 1.-lambda;
        HE->lambda2 = lambda;
        HE->Vav = -1.;
    }
    return 0;
}
double HEburn::get_lambda()
{
    if( HE == NULL )
        return 0;
    return HE->lambda2;
}

int HEburn::ddlambda(double V,   double e,  double lambda,
                          double &dP, double &dT)
{
    if( set_lambda(lambda) )
        return 1;
    if( HE->ddlambda(V,e,dP,dT) )
        return 1;
    dP = -dP;
    dT = -dT;
    return 0;
}

int HEburn::ddz(int i, double V,   double e,  const double *z,
                            double &dP, double &dT)
{
    if( i != ilambda )
        return -1;
    return ddlambda(V,e,z[ilambda], dP,dT) ? -1 : 0;  
}

double HEburn::P(double V, double e, const double *z)
{
    if( set_lambda(z[ilambda]) )
        return NaN;
    double p = HE->P(V,e);
    /****
    if( std::isnan(p) )
    {
        if( z[ilambda] < LAMBDA_MIN )
            return HE->eos1->P(V,e);
        else if( LAMBDA_MAX < z[ilambda] )
            return HE->eos2->P(V,e);
    }
    ****/
    return p;
}
double HEburn::e_PV(double p, double V, const double *z)
{
    if( set_lambda(z[ilambda]) )
        return NaN;
    return HE->e_PV(p,V);
}

double HEburn::T(double V, double e, const double *z)
{
    if( set_lambda(z[ilambda]) )
        return NaN;
    double Tz = HE->T(V,e);
    if( std::isnan(Tz) )
    {
        if( z[ilambda] < LAMBDA_MIN )
            return HE->eos1->T(V,e);
        else if( LAMBDA_MAX < z[ilambda] )
            return HE->eos2->T(V,e);
    }
    return Tz;
}

double HEburn::S(double V, double e, const double *z)
{
    if( set_lambda(z[ilambda]) )
        return NaN;
    double s = HE->S(V,e);
    if( std::isnan(s) )
    {
        if( z[ilambda] < LAMBDA_MIN )
            return HE->eos1->S(V,e);
        else if( LAMBDA_MAX < z[ilambda] )
            return HE->eos2->S(V,e);
    }
    return s;
}

double HEburn::c2(double V, double e, const double *z)
{
    if( set_lambda(z[ilambda]) )
        return NaN;
    double c_2 = HE->c2(V,e);
    /****
    if( std::isnan(c_2) || c_2 <= 0.0 )
    {
        if( z[ilambda] < LAMBDA_MIN )
            return HE->eos1->c2(V,e);
        else if( LAMBDA_MAX < z[ilambda] )
            return HE->eos2->c2(V,e);
    }
    ****/
    return c_2;
}

double HEburn::Gamma(double V, double e, const double *z)
{
    if( set_lambda(z[ilambda]) )
        return NaN;
    double G = HE->Gamma(V,e);
    if( std::isnan(G) )
    {
        if( z[ilambda] < LAMBDA_MIN )
            return HE->eos1->Gamma(V,e);
        else if( LAMBDA_MAX < z[ilambda] )
            return HE->eos2->Gamma(V,e);
    }
    return G;
}

double HEburn::CV(double V, double e, const double *z)
{
    return set_lambda(z[ilambda]) ? NaN : HE->CV(V,e);
}

double HEburn::CP(double V, double e, const double *z)
{
    return set_lambda(z[ilambda]) ? NaN : HE->CP(V,e);
}

double HEburn::KT(double V, double e, const double *z)
{
    return set_lambda(z[ilambda]) ? NaN : HE->KT(V,e);
}

double HEburn::beta(double V, double e, const double *z)
{
    return set_lambda(z[ilambda]) ? NaN : HE->beta(V,e);
}

double HEburn::FD(double V, double e, const double *z)
{
    return set_lambda(z[ilambda]) ? NaN : HE->FD(V,e);
}

int HEburn::NotInDomain(double V, double e, const double *z)
{
    return set_lambda(z[ilambda]) ? 1 : HE->NotInDomain(V,e);
}
