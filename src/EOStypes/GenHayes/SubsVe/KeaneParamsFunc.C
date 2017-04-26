#include <EOSbase.h>
#include "KeaneParams.h"


double KeaneBase::Pc(double V)
{
    if( V <= 0. )
        return EOSbase::NaN;
    if( V >= Vmax )
        return Pmin;
    double K,Kp,Kpinfty, eta,P;
    if( V > V1 )
    {
        eta     = V0/V;
        P       = P0;
        K       = K0;
        Kp      = Kp0;
        Kpinfty = Kp0infty;
    }
    else
    {
        eta     = V1/V;
        P       = P1;
        K       = K1;
        Kp      = Kp1;
        Kpinfty = Kp1infty;
    }
    double r = Kp/Kpinfty;
    return P + K * ( (r/Kpinfty)*(pow(eta,Kpinfty)-1.) -(r-1.)*log(eta) );
}

double KeaneBase::dPc(double V)
{
    if( V <= 0. )
        return EOSbase::NaN;
    if( V >= Vmax )
        return 0.;
    double K,Kp,Kpinfty, eta;
    if( V > V1 )
    {
        eta     = V0/V;
        K       = K0;
        Kp      = Kp0;
        Kpinfty = Kp0infty;
    }
    else
    {
        eta     = V1/V;
        K       = K1;
        Kp      = Kp1;
        Kpinfty = Kp1infty;
    }
    double r = Kp/Kpinfty;
    return -(K/V)*( 1.0 + r*(pow(eta,Kpinfty)-1.0) );
}

double KeaneBase::ec(double V)
{
    if( V <= 0. )
        return EOSbase::NaN;
    if( V >= Vmax )
        return emin + Pmin*(Vmax-V);
    
    double e, VK,Kp,Kpinfty, eta;
    if( V > V1 )
    {
        e       = e0 + P0*(V0-V);
        eta     = V0/V;
        VK      = V0*K0;
        Kp      = Kp0;
        Kpinfty = Kp0infty;
    }
    else
    {
        e       = e1 + P1*(V1-V);
        eta     = V1/V;
        VK      = V1*K1;
        Kp      = Kp1;
        Kpinfty = Kp1infty;
    }
    double r = Kp/Kpinfty;
    return e + VK*(
                (r/Kpinfty)*((pow(eta,Kpinfty-1)-1.)/(Kpinfty-1.)-(eta-1.)/eta)
                + (r-1.)*((1.+log(eta))/eta - 1.)
               );
}

double KeaneBase::theta(double V)
{
    if( V <= 0. )
        return EOSbase::NaN;
    if( V <= V0 )
        return theta0*pow(V0/V,a)*exp(b*(V0-V)/V0);
    else
    {
        double eps = (V/V0-1.)/eps1;
        double lnV = a*log(V/V0) + b*eps1*log(eps +sqrt(1.+sqr(eps)));
        return theta0*exp(-lnV);
    }
}

double KeaneBase::Dln_theta(double V)
{
    // Dln_theta = -(d/dV)ln(theta) = Gamma/V
    if( V <= V0 )
        return a/V + b/V0;
    else
        return a/V + (b/V0)/sqrt(1.+sqr((V/V0-1.)/eps1));
}

double KeaneBase::D2theta(double V)
{
    // D2theta = [d^2(theta)/d V^2] / theta
    if( V <= V0 )
        return (sqr(a+b*V/V0)+a)/sqr(V);
    else
    {
        double eps = (V/V0-1.)/eps1;
        double e2 = 1.+sqr(eps);
        double sqe = sqrt(e2);
        double GammaV  = a/V + (b/V0)/sqe;
        double dGammaV = a/sqr(V) +(b/sqr(V0))*eps/(eps1*sqe*e2);
        return sqr(GammaV)+dGammaV;
    }
}

double KeaneBase::SCv(double sT)
{
    if( sT < 0. )
        return EOSbase::NaN;
    return Cv*sT*sT*sT/(((cv[3]*sT+cv[2])*sT+cv[1])*sT+cv[0]);    
}

#define ERROR eos->ErrorHandler()->Log
double KeaneBase::IntSCv(double sT)
{
    if( !finite(sT) || sT < 0. || eos->status() )
        return EOSbase::NaN;
    double value;
    int status = IntSCv_ODE::IntSCv(sT,value);
    if( status )
    {
        ERROR( "KeaneBase::IntSCv", __FILE__, __LINE__, eos,
                  "ODE failed with status: %s\n", ODE::ErrorStatus(status) );
        value = EOSbase::NaN;
    }
    return value;
}

double KeaneBase::InvIntSCv(double C)
{
    if( !finite(C) || C < 0. || eos->status() )
        return EOSbase::NaN;
    double value;
    int status = IntSCv_ODE::InvIntSCv(C,value);
    if( status )
    {
        ERROR( "KeaneBase::InvIntSCv", __FILE__, __LINE__, eos,
                  "ODE failed with status: %s\n", ODE::ErrorStatus(status) );
        value = EOSbase::NaN;
    }
    return value;
}

double KeaneBase::IntSCvT(double sT)
{
    if( !finite(sT) || sT < 0. || eos->status() )
        return EOSbase::NaN;
    double value;
    int status = IntSCv_ODE::IntSCvT(sT,value);
    if( status )
    {
        ERROR( "KeaneBase::IntSCvT", __FILE__, __LINE__, eos,
                  "ODE failed with status: %s\n", ODE::ErrorStatus(status) );
        value = EOSbase::NaN;
    }
    return value;
}
