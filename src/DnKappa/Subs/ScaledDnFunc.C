#include "ScaledDn.h"

int ScaledDn::InitState(double rho, double T)
{
    rho0 = rho;
    T0   = T;
    double dr = rho0/rho0_r-1.;
    double dT = T0/T0_r-1.;
    Dcj    = (1. + c6*dr - c8*dT) * Dcj_r;
    lscale = (1. + c7*dr + (-c9+c10*dT)*dT) * lscale_r;
    if( Dcj>0. && lscale>0.) return 0.0;
    EOSstatus = EOSlib::fail;
    return 1;
}

double ScaledDn::Dn(double kappa)
{
    double ratio;
    if( kappa > kappa_max ) kappa = kappa_max;
    kappa *= lscale;
    if( kappa < 0. )
    {
        ratio = 1. - kappa;
        if( ratio > d_max ) ratio = d_max;
    }
    else
    {
        ratio = 1. - kappa*(1.+c2*kappa+c3*kappa*kappa)
                          /(1.+c4*kappa+c5*kappa*kappa);
    }
    return Dcj*ratio;
}
