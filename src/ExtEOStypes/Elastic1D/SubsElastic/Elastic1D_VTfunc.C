#include "Elastic1D_VT.h"

// EOS_VT functions

double Elastic1D_VT::P(double V, double T)
{
    return P(V,T,Eps_el(V));
}

double Elastic1D_VT::e(double V, double T)
{
    return e(V,T,Eps_el(V));
}

double Elastic1D_VT::S(double V, double T)
{
    return S(V,T,Eps_el(V));
}

double Elastic1D_VT::P_V(double V, double T)
{
    return P_V(V,T,Eps_el(V));
}

double Elastic1D_VT::P_T(double V, double T)
{
    return P_T(V,T,Eps_el(V));
}

double Elastic1D_VT::CV(double V, double T)
{
    return CV(V,T,Eps_el(V));
}

double Elastic1D_VT::F(double V, double T)
{
    return F(V,T,Eps_el(V));
}

int Elastic1D_VT::NotInDomain(double V, double T)
{
    return NotInDomain(V,T,Eps_el(V));
}


// elastic EOS_VT functions

double Elastic1D_VT::P(double V, double T, double eps_el)
{
    if( hydro == NULL || shear == NULL )
        return NaN;
    return hydro->P(V,T)+shear->Ps(V,T,eps_el)+shear->Pdev(V,T,eps_el);
}
    
double Elastic1D_VT::e(double V, double T, double eps_el)
{
    if( hydro == NULL || shear == NULL )
        return NaN;
    return hydro->e(V,T)+shear->e(V,T,eps_el);
}
    
double Elastic1D_VT::S(double V, double T, double eps_el)
{
    if( hydro == NULL || shear == NULL )
        return NaN;
    return hydro->S(V,T)+shear->S(V,T,eps_el);
}
    
double Elastic1D_VT::P_V(double V, double T, double eps_el)
{
    if( hydro == NULL || shear == NULL )
        return NaN;
    return hydro->P_V(V,T)-shear->cT2(V,T,eps_el)/(V*V);
}
    
double Elastic1D_VT::P_T(double V, double T, double eps_el)
{
    if( hydro == NULL || shear == NULL )
        return NaN;
    return hydro->P_T(V,T)+shear->dPdT(V,T,eps_el);
}
    
double Elastic1D_VT::CV(double V, double T, double eps_el)
{
    if( hydro == NULL || shear == NULL )
        return NaN;
    return hydro->CV(V,T)+shear->CV(V,T,eps_el);
}
    
double Elastic1D_VT::F(double V, double T, double eps_el)
{
    if( hydro == NULL || shear == NULL )
        return NaN;
    return hydro->F(V,T)+shear->F(V,T,eps_el);
}
    
int Elastic1D_VT::NotInDomain(double V, double T, double eps_el)
{
    if( hydro == NULL || shear == NULL )
        return 1;
    if( V<=0 || T<0 )
        return 1;
    double c2 = hydro->cT2(V,T);
    if( std::isnan(c2) || c2 < 0 )
        return 1;
    c2 = shear->cT2(V,T,eps_el);
    return std::isnan(c2) || c2 < 0;
}
