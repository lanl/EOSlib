#include "ElasticConstGV_VT.h"

// elastic EOS_VT functions

double ElasticConstGV_VT::P(double V, double T, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return NaN;
    return hydro->P(V,T)+GV->Pdev(V,T,eps_el);
}
    
double ElasticConstGV_VT::e(double V, double T, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return NaN;
    return hydro->e(V,T)+GV->e(V,T,eps_el);
}
    
double ElasticConstGV_VT::S(double V, double T, double)
{
    if( hydro == NULL )
        return NaN;
    return hydro->S(V,T);
}
    
double ElasticConstGV_VT::P_V(double V, double T, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return NaN;
    return hydro->P_V(V,T)-GV->cT2(V,T,eps_el)/(V*V);
}
    
double ElasticConstGV_VT::P_T(double V, double T, double)
{
    if( hydro == NULL )
        return NaN;
    return hydro->P_T(V,T);
}
    
double ElasticConstGV_VT::CV(double V, double T, double eps_el)
{
    if( hydro == NULL )
        return NaN;
    return hydro->CV(V,T);
}
    
double ElasticConstGV_VT::F(double V, double T, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return NaN;
    return hydro->F(V,T)+GV->F(V,T,eps_el);
}
    
int ElasticConstGV_VT::NotInDomain(double V, double T, double eps_el)
{
    if( hydro == NULL || GV == NULL )
        return 1;
    if( V<=0 || T<0 )
        return 1;
    double c2 = hydro->cT2(V,T);
    if( std::isnan(c2) || c2 < 0 )
        return 1;
    c2 = GV->cT2(V,T,eps_el);
    return std::isnan(c2) || c2 < 0;
}
