#include "Elastic1D.h"

// EOS Functions

double Elastic1D::P(double V, double e)
{
    return P(V,e,Eps_el(V));
}

double Elastic1D::T(double V, double e)
{
    return T(V,e,Eps_el(V));
}

double Elastic1D::S(double V, double e)
{
    return S(V,e,Eps_el(V));
}

double Elastic1D::c2(double V, double e)
{
    return c2(V,e,Eps_el(V));
}

double Elastic1D::Gamma(double V, double e)
{
    return Gamma(V,e,Eps_el(V));
}

double Elastic1D::CV(double V, double e)
{
    return CV(V,e,Eps_el(V));
}

double Elastic1D::FD(double V, double e)
{
    return EOS::FD(V,e);
}

int Elastic1D::NotInDomain(double V, double e)
{
    return NotInDomain(V,e,Eps_el(V));
}

// elastic EOS functions

double Elastic1D::P(double V, double e, double eps_el)
{
    double Te = T(V,e,eps_el);
    if( std::isnan(Te) )
        return NaN;
    double e_hyd = e - shear->e(V,Te,eps_el);
    return hydro->P(V,e_hyd)+shear->Ps(V,Te,eps_el)+shear->Pdev(V,Te,eps_el);
}

double Elastic1D::Phydro(double V, double e, double eps_el)
{
    double Te = T(V,e,eps_el);
    if( std::isnan(Te) )
        return NaN;
    double e_hyd = e - shear->e(V,Te,eps_el);
    return hydro->P(V,e_hyd);
}

double Elastic1D::Pshear(double V, double e, double eps_el)
{
    double Te = T(V,e,eps_el);
    if( std::isnan(Te) )
        return NaN;
    return shear->Ps(V,Te,eps_el);
}

double Elastic1D::Pdev(double V, double e, double eps_el)
{
    double Te = T(V,e,eps_el);
    if( std::isnan(Te) )
        return NaN;
    return shear->Pdev(V,Te,eps_el);
}

double Elastic1D::T(double V, double e, double eps_el)
{
    if( hydro == NULL || shear == NULL )
        return NaN;
 // solve, e = hydro->e(V, T) + shear->e(V,T,eps_el))
    double T1 = hydro->T(V, e);
    // e1 = e + eshear1;
    double de_1 = shear->e(V,T1, eps_el);               // e1 - e
    if( abs(de_1) <= 1e-6*abs(e) )
        return T1;
    double T2 = hydro->T(V, e-de_1);
    // e2 = (e-de_1) + eshear2
    double de_2 = de_1  - shear->e(V, T2, eps_el);      // e - e2
 // Assume eshear > 0 and interpolate (T1,e1) and (T2,e2)
 //  T = ((e1-e)*T2+(e-e2)*T1)/(e1-e2)
 // Accurate if eshear is small, otherwise would need to iterate
    return (de_1*T2 + de_2*T1)/(de_1+de_2);
 // Alternative, solve T - hydro->T(V,e-shear->e(V,T)) = 0
 // with Newton iteration using hydro->Cv and shear->Cv for derivatives.
}

double Elastic1D::S(double V, double e, double eps_el)
{
    double Te = T(V,e,eps_el);
    if( std::isnan(Te) )
        return NaN;
    double e_hyd = e - shear->e(V,Te,eps_el);
    return hydro->S(V,e_hyd)+shear->S(V,Te,eps_el);
}

double Elastic1D::CV(double V, double e, double eps_el)
{
    double Te = T(V,e,eps_el);
    if( std::isnan(Te) )
        return NaN;
    double e_hyd = e - shear->e(V,Te,eps_el);
    return hydro->CV(V,e_hyd)+shear->CV(V,Te,eps_el);
}

double Elastic1D::c2(double V, double e, double eps_el)
{
    double  Te = T(V,e,eps_el);
    if( std::isnan(Te) )
        return NaN;
    double   e_hyd = e - shear->e(V,Te,eps_el);
    double  CV_hyd = hydro->CV(V,e_hyd);
    double   G_hyd = hydro->Gamma(V,e_hyd);
    double CT2_hyd = hydro->c2(V,e_hyd) - G_hyd*G_hyd*CV_hyd*Te;
    
    double CV_sh = shear->CV(V,Te,eps_el);
    double Gamma = (CV_hyd*G_hyd+V*shear->dPdT(V,Te,eps_el))/(CV_hyd+CV_sh);  
    return CT2_hyd+shear->cT2(V,Te,eps_el) + Gamma*Gamma*Te*(CV_hyd+CV_sh);
}

double Elastic1D::Gamma(double V, double e, double eps_el)
{
    double  Te = T(V,e,eps_el);
    if( std::isnan(Te) )
        return NaN;
    double  e_hyd = e - shear->e(V,Te,eps_el);
    double CV_hyd = hydro->CV(V,e_hyd);
    return (CV_hyd*hydro->Gamma(V,e_hyd)+V*shear->dPdT(V,Te,eps_el))
           / (CV_hyd+shear->CV(V,Te,eps_el));
}

int Elastic1D::NotInDomain(double V, double e, double eps_el)
{
    if( V <= 0 )
        return 1;
    double Te = T(V,e,eps_el);
    return std::isnan(Te) || Te < 0; 
}

//

double Elastic1D::KT(double V, double e, double eps_el)
{
    double  Te = T(V,e,eps_el);
    if( std::isnan(Te) )
        return NaN;
    double   e_hyd = e - shear->e(V,Te,eps_el);
    return hydro->KT(V,e_hyd)+shear->cT2(V,Te,eps_el)/V;
}

double Elastic1D::CP(double V, double e, double eps_el)
{
    if( hydro == NULL || shear == NULL )
        return NaN;
    return CV(V,e,eps_el)*c2(V,e,eps_el)/(V*KT(V,e,eps_el));
}

double Elastic1D::beta(double V, double e, double eps_el)
{
    if( hydro == NULL || shear == NULL )
        return NaN;
    return Gamma(V,e,eps_el)*CV(V,e,eps_el)/(V*KT(V,e,eps_el));
}

double Elastic1D::FD(double V, double e, double eps_el)
{
    if( hydro == NULL || shear == NULL )
        return NaN;
    double  eps_el_0 = eps_el_ref;
    eps_el_ref = log(V/V_ref) - eps_el;
    double fd = EOS::FD(V,e);
    eps_el_ref = eps_el_0;
    return fd;
/**************************************************************************
* To Do,
*   Replace with finite difference based on
*   FD = -0.5*(d/dV)_S[(rho*c)^2] / (rho*c)^2
*   (d/dV)_S = (d/dV)_T + (dT/dV)_S * (d/dT)_V
*            = (d/dV)_T - Gamma*(T/V)(d/dT)_V
*   and Gamma = -(V/T)(dT/dV), const S
*             = (CV_hyd*Gamma_hyd + V*dPdT_shear)/(CV_hyd+CV_shear)
*   de_hydro = CV_hyd*dT -(P_hyd -(T/V)*CV_hyd*Gamma_hyd)*dV
*   dT       = -[(CV_hyd*Gamma_hyd+V*dPdT_shear)/(CV_hyd+CV_shear)]*T*dV/V
**************************************************************************/
}
