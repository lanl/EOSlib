#include <EOS.h>
#include "Sesame.h"
#include "SesSubs.h"

int Sesame::NotInDomain(double V, double e)
{
    //if( V == Vcache && e == ecache )
    //    return 0;
    if( std::isnan(V) || isnan(e) )  // sanity check
        return 1;
    double rho = 1./V;
    int i = SESsrch(rho,rho_grd,rho_n);
    if( i<0 || i>= rho_n-1 )
        return 1;
    double e0[2];
    int k = 1;
    SESratfn1(rho,rho_grd,i, rho_n,k, e0_grd,k, e0);
    double de = e - e0[0];
    if( de < de_grd[0] || de_grd[e_n-1] < de )
        return 1;
    return 0;  
}

    
int Sesame::Cache(double V, double e)
{
    if( EOSstatus != EOSlib::good )
        return 1;
    if( V == Vcache && e == ecache )
        return 0;

    double rho = 1./V;
    int i = SESsrch(rho, rho_grd, rho_n);
    if( i < 0 )
        i = 0;
    else if( i > rho_n-2 )
        i = rho_n - 2;
    double e0[2];
    int k = 1;
    SESratfn1(rho,rho_grd,i, rho_n,k, e0_grd,k, e0);
    double de = e - e0[0];
    int j = SESsrch(de, de_grd, e_n);
    if( j < 0 )
        j = 0;
    else if( j > e_n-2 )
        j = e_n - 2;

    SESratfn2(rho,de, rho_grd,i,rho_n, de_grd,j,e_n, P_table, interp, Pcache);
    Pcache[1] -= e0[1]*Pcache[2];
    Pcache[1] /= -V*V;
    
    SESratfn2(rho,de, rho_grd,i,rho_n, de_grd,j,e_n, T_table, interp, Tcache);
    Tcache[1] -= e0[1]*Tcache[2];
    Tcache[1] /= -V*V;

    Vcache = V;
    ecache = e;   
    return 0;
}

double Sesame::P(double V, double e)
{
    if( Cache(V,e) )
        return NaN;
    return Pcache[0];
}

double Sesame::T(double V, double e)
{
    if( Cache(V,e) )
        return NaN;
    return Tcache[0];
}

double Sesame::S(double V, double e)
{
    if( Cache(V,e) )
        return NaN;
    return ses_VT->S(V,Tcache[0]);
}
    
double Sesame::c2(double V, double e)
{
    if( Cache(V,e) )
        return NaN;
    return -V*V*(Pcache[1] -Pcache[0]*Pcache[2]);
}

double Sesame::Gamma(double V, double e)
{
    if( Cache(V,e) )
        return NaN;
    return V*Pcache[2];
}

double Sesame::CV(double V, double e)
{
    if( Cache(V,e) )
        return NaN;
    return 1./Tcache[2];
}

/*****
int Sesame::PT(double P, double T, HydroState &state)
{
    if( EOSstatus != EOSlib::good )
        return 1;

    double V = ses_VT->V(P,T);
    if( std::isnan(V) )
        return 1;

    state.V = V;
    state.e = ses_VT->e(V,T);

    return 0;  
}
****/
