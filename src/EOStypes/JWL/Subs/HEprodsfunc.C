#include "HEprods.h"

double HEprods::P(double V, double e)
{
    double P_ref = Pref(V);
    if( std::isnan(P_ref) ) return NaN;
    return P_ref + GammaVref(V)*(e-eref(V));
}
double HEprods::e_PV(double p, double V)
{
    double P_ref = Pref(V);
    if( std::isnan(P_ref) ) return NaN;

    double GammaV = GammaVref(V);
    return (p-P_ref)/GammaV + eref(V);
}

double HEprods::c2(double V, double e)
{
    double c2_ref = c2ref(V);
    if( std::isnan(c2_ref) || c2_ref<0. ) return NaN;
    double Gamma = V*GammaVref(V);
    double dGammaV = dGammaVref(V);

    return c2_ref + (Gamma*Gamma+dGammaV)*(e-eref(V));
}

/***
double HEprods::FD(double V, double e)
{
    if( V <= 0. )
        return NaN;
}
***/

double HEprods::Gamma(double V, double e)
{
    double GammaV = GammaVref(V);
    return std::isnan(GammaV) ?  NaN : V*GammaV;
}

//


int HEprods::NotInDomain(double V, double e)
{
    double c2_Ve = c2(V,e);
    if( std::isnan(c2_Ve) ) return NaN;
    return (c2_Ve>0. && P(V,e)>0.) ? 0 : 1;
}

// Thermal properties

double HEprods::T(double V, double e)
{
    return Tref(V) + (e-eref(V))/Cv;
}

double HEprods::S(double V, double e)
{
    double T_Ve = T(V,e);
    return (std::isnan(T_Ve) && T_Ve<=0.) ? NaN : Cv*log(T_Ve/T_cj) + S_cj;
}

double HEprods::CV(double, double)
{
    return Cv;
}

// Reference curve (CJ isentrope)

double HEprods::Pref(double V)
{
    if( V <= 0. )
        return NaN;
    double rho = 1./V;
    if( rho < rho_min || rho > rho_max )
        return NaN;

    int i,j;
    double P = 0.0;
    if( rho < rho_sw )
    {
        for( i=0; i<na1; ++i )
        {
            j = na1-1 -i;
            P = rho*P + a1[j];
        }
        P *= pow(rho,gamma1);
    }
    else
    {
        for( i=0; i<na2; ++i )
        {
            j = na2-1 -i;
            P = rho*P + a2[j];
        }
        P *= pow(rho,gamma2);
    }
    return P;
}

double HEprods::eref(double V)
{
    // eref = e_cj + int^rho_rhocj drho Pref(rho)/rho^2
    //      = e_cj + sum_0..n{int^rho_rhocj drho  an*rho^(n+gamma-2)}
    //      = e_cj + sum_0..n an/(n+gamma-1)*rho^(n+gamma-1)
    //      = e_cj + [sum_0..n an/(n+gamma-1)*rho^n]* rho^(gamma-1)
    // assume gamma > 1, then n+gamma-1 > 0

    if( V <= 0. )
        return NaN;
    double rho = 1./V;
    if( rho < rho_min || rho > rho_max )
        return NaN;

    double e1=0.0;
    double e2=0.0;
    double e;
    int i,j;
    if( rho < rho_sw )
    {
        for( i=0; i<na1; ++i )
        {
            j = na1-1 -i;
            e1 = rho*e1    + a1[j]/(j-1+gamma1);
            e2 = rho_sw*e2 + a1[j]/(j-1+gamma1);
        }
        e = e_sw + e1*pow(rho,gamma1-1.) - e2*pow(rho_sw,gamma1-1.);
    }
    else
    {
        for( i=0; i<na2; ++i )
        {
            j = na2-1 -i;
            e1 = rho*e1    + a2[j]/(j-1+gamma2);
            e2 = rho_cj*e2 + a2[j]/(j-1+gamma2);
        }
        e = e_cj + e1*pow(rho,gamma2-1.) - e2*pow(rho_cj,gamma2-1.);
    }
    return e;
}

double HEprods::c2ref(double V)
{
    if( V <= 0. )
        return NaN;
    double rho = 1./V;
    if( rho < rho_min || rho > rho_max )
        return NaN;

    double c2  = 0.;
    double c2p = 0.;
    int i,j,n;
    if( rho < rho_sw )
    {
        n = na1-1;
        for( i=0; i<n; ++i )
        {
            j = n-i;
            c2p = rho*c2p + a1[j];
            c2  = rho*c2 + j*a1[j];
        }
        c2p = rho*c2p + a1[0];
        c2  = pow(rho,gamma1)*(c2 + gamma1*c2p/rho);
    }
    else
    {
        n = na2-1;
        for( i=0; i<n; ++i )
        {
            j = n -i;
            c2p = rho*c2p + a2[j];
            c2  = rho*c2 + j*a2[j];
        }
        c2p = rho*c2p + a2[0];
        c2  = pow(rho,gamma2)*(c2 + gamma2*c2p/rho);
    }
    return c2;
}

double HEprods::GammaVref(double V)
{
    // GammaVref = rho*Gamma_ref
    if( V <= 0. )
        return NaN;
    double rho = 1./V;
    if( rho < rho_min || rho > rho_max )
        return NaN;

    double GammaV = 0.;
    int i,j;
    if( rho < rho_sw )
    {
        for( i=0; i<nb1; ++i )
        {
            j = nb1-i;
            GammaV  = rho*GammaV + b1[j];
        }
    }
    else
    {
        for( i=0; i<nb2; ++i )
        {
            j = nb2 -i;
            GammaV  = rho*GammaV + b2[j];
        }
    }
    return GammaV;
}

double HEprods::dGammaVref(double V)
{
    // d(rho*Gamma)/drho
    if( V <= 0. )
        return NaN;
    double rho = 1./V;
    if( rho < rho_min || rho > rho_max )
        return NaN;

    double GammaV = 0.;
    int i,j,n;
    if( rho < rho_sw )
    {
        n = nb1-1;
        for( i=0; i<n; ++i )
        {
            j = n-i;
            GammaV  = rho*GammaV + j*b1[j];
        }
    }
    else
    {
        n = nb2-1;
        for( i=0; i<n; ++i )
        {
            j = n -i;
            GammaV  = rho*GammaV + j*b2[j];
        }
    }
    return GammaV;
}

double HEprods::Tref(double V)
{
    if( V <= 0. )
        return NaN;
    double rho = 1./V;
    if( rho < rho_min || rho > rho_max )
        return NaN;

    // T = T_cj * exp{int^Vcj_V Gamma(V) dV/V}
    //   = T_cj * exp{int^rho_rhocj (Gamma(rho)/rho)  drho}
    // integral = sum_0..n int^rho_rhocj bn*rho^(n-2) drho
    //          = -b0/rho + b1*ln(rho) + [sum_2..n  bn/(n-1)*rho^n]/rho 
    double T_ref;
    double T1 = 0.;
    double T2 = 0.;
    int i,j;
    if( rho < rho_sw )
    {
        for( i=0; i<nb1-2; ++i )
        {
            j = nb1-1-i;
            T1 = rho*T1    + b1[j]/(j-1);
            T2 = rho_sw*T2 + b1[j]/(j-1);
        }
        T1 = (-b1[0]+T1)/rho;
        T2 = (-b1[0]+T2)/rho_sw;
        T_ref = T_sw*exp(T1-T2 + b1[1]*log(rho/rho_sw));
    }
    else
    {
        for( i=0; i<nb2-2; ++i )
        {
            j = nb2-1-i;
            T1 = rho*T1    + b2[j]/(j-1);
            T2 = rho_cj*T2 + b2[j]/(j-1);
        }
        T1 = (-b2[0]+T1)/rho;
        T2 = (-b2[0]+T2)/rho_cj;
        T_ref = T_cj*exp(T1-T2 + b2[1]*log(rho/rho_cj));
    }
    return T_ref;
}
//
// functions to calculate sonic state on CJ isentrope
//
double HEprods::fsonic(double rho)
{
    double V = 1./rho;
    double P = Pref(V);
    double c2 = c2ref(V);
    double R = (P-P0)/(V0-V);
    
    return (R - rho*rho*c2)*V0/P;
}

double HEprods::FindCJ()
{
    double tol = 1e-6;

    double rho, rho1, rho2;
    double f;
    int i;
    // Find bounds
    rho1 = rho_sw;
    for( i=20; i; --i )
    {
        rho2 = 1.1*rho1;
        f = fsonic(rho2);
        if( f < 0. ) break;
        rho1 = rho2;
    }
    if( i == 0 ) return NaN;
    // bisection
    for( i=40; i; --i )
    {
        rho = 0.5*(rho1+rho2);
        f = fsonic(rho);
        if( f > 0. )
        {
            if( f < tol ) break;
            rho1 = rho;
        }
        else
        {
            if( -f < tol ) break;
            rho2 = rho;
        }
    }
    if( i == 0 ) return NaN;
    return rho;
}
