#include "StiffIdealHEav.h"

double *StiffIdealHEav::z_f(double V, double e)
{
    if( frozen )
    {
        ztmp[0] = (*z_ref)(0);
        ztmp[1] = (*z_ref)(1);
    }
    else
    {
        ztmp[0] = 1.0;
        ztmp[1] = HE ? (e+HE->Q)/HE->Cv2 : (*z_ref)(1);
    }
    return ztmp;        
}

int StiffIdealHEav::set_lambda(double lambda)
{
    if( HE == NULL )
        return 1;
    HE->set_lambda2(lambda);
    return 0;
}
double StiffIdealHEav::get_lambda()
{
    if( HE == NULL )
        return 0;
    return HE->lambda2;
}

int StiffIdealHEav::ddlambda(double V,   double e,  double lambda,
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

int StiffIdealHEav::ddz(int i, double V,   double e,  const double *z,
                            double &dP, double &dT)
{
    if( i == 0 )
        return ddlambda(V,e,z[0], dP,dT) ? -1 : 0;
    else if( i==1 )
    {
        dP = dT = 0.0;
        return 0;
    }
    else
        return -1;
}

double StiffIdealHEav::P(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->P(V,e);
}

double StiffIdealHEav::T(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->T(V,e);
}

double StiffIdealHEav::S(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->S(V,e);
}

double StiffIdealHEav::c2(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->c2(V,e);
}

double StiffIdealHEav::Gamma(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->Gamma(V,e);
}

double StiffIdealHEav::CV(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->CV(V,e);
}

double StiffIdealHEav::CP(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->CP(V,e);
}

double StiffIdealHEav::KT(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->KT(V,e);
}

double StiffIdealHEav::beta(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->beta(V,e);
}

double StiffIdealHEav::FD(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? NaN : HE->FD(V,e);
}

int StiffIdealHEav::NotInDomain(double V, double e, const double *z)
{
    return set_lambda(z[0]) ? 1 : HE->NotInDomain(V,e);
}
