#include "ElasticPolyG_VT.h"

// elastic EOS_VT functions

double ElasticPolyG_VT::P(double V, double T, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return NaN;
    return hydro->P(V,T)+PG->Pdev(V,T,eps_el)+PG->Ps(V,T,eps_el);
}
    
double ElasticPolyG_VT::e(double V, double T, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return NaN;
    return hydro->e(V,T)+PG->e(V,T,eps_el);
}
    
double ElasticPolyG_VT::S(double V, double T, double)
{
    if( hydro == NULL )
        return NaN;
    return hydro->S(V,T);
}
    
double ElasticPolyG_VT::P_V(double V, double T, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return NaN;
    return hydro->P_V(V,T)-PG->cT2(V,T,eps_el)/(V*V);
}
    
double ElasticPolyG_VT::P_T(double V, double T, double)
{
    if( hydro == NULL )
        return NaN;
    return hydro->P_T(V,T);
}
    
double ElasticPolyG_VT::CV(double V, double T, double eps_el)
{
    if( hydro == NULL )
        return NaN;
    return hydro->CV(V,T);
}
    
double ElasticPolyG_VT::F(double V, double T, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return NaN;
    return hydro->F(V,T)+PG->F(V,T,eps_el);
}
    
int ElasticPolyG_VT::NotInDomain(double V, double T, double eps_el)
{
    if( hydro == NULL || PG == NULL )
        return 1;
    if( V<=0 || T<0 )
        return 1;
    double c2 = hydro->cT2(V,T);
    if( isnan(c2) || c2 < 0 )
        return 1;
    c2 = PG->cT2(V,T,eps_el);
    return isnan(c2) || c2 < 0;
}
