#include "Elastic.h"

IDOF *Elastic_VT::z_ref()
{
    return static_cast<IDOF *>(this);
}

double *Elastic_VT::z_f(double V, double)
{
    ztmp[0] = eps_el(V);
    return ztmp;
}
double Elastic_VT::eshear(double V, double T, double eps_el)
{
    return Fshear(V,T, eps_el) + T*Sshear(V,T, eps_el);
}

double Elastic_VT::stress(double V, double T, double eps_el)
{
    return Pshear(V,T, eps_el) + (2./3.)*shear(V,T, eps_el);
}

int Elastic_VT::Rate(double V, double T, const double *z, double *zdot)
{
    return 1;
}
//
//
double Elastic_VT::F(double V, double T, const double *z)
{
    if( EOSstatus != EOSlib::good )
        return NaN;
    return hydro->F(V,T) + Fshear(V,T,*z);
}

double Elastic_VT::P(double V, double T, const double *z)
{
    if( EOSstatus != EOSlib::good )
        return NaN;
    return hydro->P(V,T) - Pshear(V,T, *z) - (2./3.)*shear(V,T, *z);    
}

double Elastic_VT::e(double V, double T, const double *z)
{
    if( EOSstatus != EOSlib::good )
        return NaN;
    return hydro->e(V,T) + eshear(V,T,*z);
}

double Elastic_VT::S(double V, double T, const double *z)
{
    if( EOSstatus != EOSlib::good )
        return NaN;
    return hydro->S(V,T) + Sshear(V,T,*z);
}

double Elastic_VT::P_V(double V, double T, const double *z)
{
    if( EOSstatus != EOSlib::good )
        return NaN;
    return hydro->P_V(V,T) - stress_V(V,T,*z);
}

double Elastic_VT::P_T(double V, double T, const double *z)
{
    if( EOSstatus != EOSlib::good )
        return NaN;
    return hydro->P_T(V,T) - stress_T(V,T,*z);
}

double Elastic_VT::CV(double V, double T, const double *z)
{
    if( EOSstatus != EOSlib::good )
        return NaN;
    return hydro->CV(V,T) + CVshear(V,T,*z);
}

int Elastic_VT::NotInDomain(double V, double T, const double *z)
{
    if( EOSstatus != EOSlib::good )
        return 1;
    return hydro->NotInDomain(V,T);  
}
