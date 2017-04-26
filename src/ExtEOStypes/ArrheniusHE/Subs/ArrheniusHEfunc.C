#include "ArrheniusHE.h"

double *ArrheniusHE::z_f(double V, double e)
{
    ztmp[0] = frozen ? (*z_ref)(0) : 1;
    return ztmp;        
}

int ArrheniusHE::set_lambda(double lambda)
{
    if( HE == NULL )
        return 1;
    HE->lambda1 = 1. - lambda;
    HE->lambda2 = lambda;
    HE->Vav = -1.;
    return 0;
}
double ArrheniusHE::get_lambda()
{
    if( HE == NULL )
        return 0;
    return HE->lambda2;
}

int ArrheniusHE::ddlambda(double V,   double e,  double lambda,
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

int ArrheniusHE::ddz(int i, double V,   double e,  const double *z,
                            double &dP, double &dT)
{
    if( i != 0 )
        return -1;
    return ddlambda(V,e,z[0], dP,dT) ? -1 : 0;  
}

double ArrheniusHE::P(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->P(V,e);
}

double ArrheniusHE::T(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->T(V,e);
}

double ArrheniusHE::S(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->S(V,e);
}

double ArrheniusHE::c2(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->c2(V,e);
}

double ArrheniusHE::Gamma(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->Gamma(V,e);
}

double ArrheniusHE::CV(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->CV(V,e);
}

double ArrheniusHE::CP(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->CP(V,e);
}

double ArrheniusHE::KT(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->KT(V,e);
}

double ArrheniusHE::beta(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->beta(V,e);
}

double ArrheniusHE::FD(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->FD(V,e);
}

int ArrheniusHE::NotInDomain(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? 1 : HE->NotInDomain(V,e);
}
