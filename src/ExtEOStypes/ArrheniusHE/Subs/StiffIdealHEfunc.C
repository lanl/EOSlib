#include "StiffIdealHE.h"

double *StiffIdealHE::z_f(double V, double e)
{
    ztmp[0] = frozen ? (*z_ref)(0) : 1;
    return ztmp;        
}

int StiffIdealHE::set_lambda(double lambda)
{
    if( HE == NULL )
        return 1;
    HE->set_lambda2(lambda);
    return 0;
}
double StiffIdealHE::get_lambda()
{
    if( HE == NULL )
        return 0;
    return HE->lambda2;
}

int StiffIdealHE::ddlambda(double V,   double e,  double lambda,
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

int StiffIdealHE::ddz(int i, double V,   double e,  const double *z,
                            double &dP, double &dT)
{
    if( i != 0 )
        return -1;
    return ddlambda(V,e,z[0], dP,dT) ? -1 : 0;  
}

double StiffIdealHE::P(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->P(V,e);
}

double StiffIdealHE::T(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->T(V,e);
}

double StiffIdealHE::S(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->S(V,e);
}

double StiffIdealHE::c2(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->c2(V,e);
}

double StiffIdealHE::Gamma(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->Gamma(V,e);
}

double StiffIdealHE::CV(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->CV(V,e);
}

double StiffIdealHE::CP(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->CP(V,e);
}

double StiffIdealHE::KT(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->KT(V,e);
}

double StiffIdealHE::beta(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->beta(V,e);
}

double StiffIdealHE::FD(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->FD(V,e);
}

int StiffIdealHE::NotInDomain(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? 1 : HE->NotInDomain(V,e);
}
