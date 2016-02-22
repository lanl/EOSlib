#include <EOS_VT.h>
#include "Sesame_VT.h"
#include "SesSubs.h"

int Sesame_VT::Cache(double V, double T)
{
    if( EOSstatus != EOSlib::good )
        return 1;
    if( V == Vcache && T == Tcache )
        return 0;
    
    Vcache = V;
    Tcache = T;
    
    double rho = 1/V;
    int i = SESsrch(rho, rho_grd, rho_n);
    if( i < 0 )
        i = 0;
    else if( i > rho_n-2 )
        i = rho_n - 2;
    int j = SESsrch(T, T_grd, T_n);
    if( j < 0 )
        j = 0;
    else if( j > T_n-2 )
        j = T_n - 2;

    SESratfn2(rho,T, rho_grd,i,rho_n, T_grd,j,T_n, P_table, interp, Pcache);
    Pcache[1] /= -V*V;
    SESratfn2(rho,T, rho_grd,i,rho_n, T_grd,j,T_n, e_table, interp, ecache);
    ecache[1] /= -V*V;
    return 0;
}

int Sesame_VT::NotInDomain(double V, double T)
{
    if( EOSstatus != EOSlib::good )
        return 1;
    double rho = 1/V;
    return rho < rho_grd[0] || rho_grd[rho_n-1] < rho
           || T < T_grd[0]  || T_grd[T_n-1] < T;
}

double Sesame_VT::P(double V, double T)
{
    if( Cache(V,T) )
        return NaN;
    return Pcache[0];
}

double Sesame_VT::e(double V, double T)
{
    if( Cache(V,T) )
        return NaN;
    return ecache[0];
}

double Sesame_VT::S(double V, double T)
{
    if( EOSstatus != EOSlib::good )
        return NaN;
    
    double rho = 1/V;
    int i = SESsrch(rho, rho_grd, rho_n);
    if( i < 0 )
        i = 0;
    else if( i > rho_n-2 )
        i = rho_n - 2;
    int j = SESsrch(T, T_grd, T_n);
    if( j < 0 )
        j = 0;
    else if( j > T_n-2 )
        j = T_n - 2;

    double s[3];
    SESratfn2(rho,T, rho_grd,i,rho_n, T_grd,j,T_n, S_table, interp, s);
    return s[0];
}

double Sesame_VT::P_V(double V, double T)
{
    if( Cache(V,T) )
        return NaN;
    return Pcache[1];
}

double Sesame_VT::P_T(double V, double T)
{
    if( Cache(V,T) )
        return NaN;
    return Pcache[2];
}

double Sesame_VT::CV(double V, double T)
{
    if( Cache(V,T) )
        return NaN;
    return ecache[2];
}
