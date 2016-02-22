#include <EOSbase.h>
#include "BirchMurnaghanParams.h"


double BirchMurnaghanBase::Pc(double V)
{
    if( V <= 0. )
        return EOSbase::NaN;
    if( V >= Vmax )
        return Pmin;
    double eta = V0/V;
    double eta2 = pow(eta,(2./3.));
    double eta5 = eta*eta2;
    double eta7 = eta5*eta2;
    return 1.5*K0*(eta7-eta5)*(1.+0.75*(Kp0-4.)*(eta2-1.));
}

double BirchMurnaghanBase::dPc(double V)
{
    if( V <= 0. )
        return EOSbase::NaN;
    if( V >= Vmax )
        return 0.;
    double eta = V0/V;
    double eta2 = pow(eta,(2./3.));
    double eta5 = eta*eta2;
    double eta7 = eta5*eta2;
    return -(K0/V)*( (3.5*eta7-2.5*eta5)*(1.+0.75*(Kp0-4.)*(eta2-1.))
                      + 0.75*(eta7-eta5)*(Kp0-4.)*eta2 );
}

double BirchMurnaghanBase::ec(double V)
{
    if( V <= 0. )
        return EOSbase::NaN;
    if( V >= Vmax )
        return e1 + Pmin*(Vmax-V);
    
    double eta = V0/V;
    double eta2 = pow(eta,(2./3.));
    double eta4 = eta2*eta2;
    return e0 + (9./8.)*V0*K0*( eta4-2.*eta2+1.
            +0.5*(Kp0-4.)*(eta*eta+3.*eta2-3.*eta4-1.) );
}

double BirchMurnaghanBase::theta(double V)
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

double BirchMurnaghanBase::Dln_theta(double V)
{
    // Dln_theta = -(d/dV)ln(theta) = Gamma/V
    if( V <= V0 )
        return a/V + b/V0;
    else
        return a/V + (b/V0)/sqrt(1.+sqr((V/V0-1.)/eps1));
}

double BirchMurnaghanBase::D2theta(double V)
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

double BirchMurnaghanBase::SCv(double sT)
{
    if( sT < 0. )
        return EOSbase::NaN;
    return Cv*sT*sT*sT/(((cv[3]*sT+cv[2])*sT+cv[1])*sT+cv[0]);    
}

#define ERROR eos->ErrorHandler()->Log
double BirchMurnaghanBase::IntSCv(double sT)
{
    if( !finite(sT) || sT < 0. || eos->status() )
        return EOSbase::NaN;
    double value;
    int status = IntSCv_ODE::IntSCv(sT,value);
    if( status )
    {
        ERROR( "BirchMurnaghanBase::IntSCv", __FILE__, __LINE__, eos,
                  "ODE failed with status: %s\n", ODE::ErrorStatus(status) );
        value = EOSbase::NaN;
    }
    return value;
}

double BirchMurnaghanBase::InvIntSCv(double C)
{
    if( !finite(C) || C < 0. || eos->status() )
        return EOSbase::NaN;
    double value;
    int status = IntSCv_ODE::InvIntSCv(C,value);
    if( status )
    {
        ERROR( "BirchMurnaghanBase::InvIntSCv", __FILE__, __LINE__, eos,
                  "ODE failed with status: %s\n", ODE::ErrorStatus(status) );
        value = EOSbase::NaN;
    }
    return value;
}

double BirchMurnaghanBase::IntSCvT(double sT)
{
    if( !finite(sT) || sT < 0. || eos->status() )
        return EOSbase::NaN;
    double value;
    int status = IntSCv_ODE::IntSCvT(sT,value);
    if( status )
    {
        ERROR( "BirchMurnaghanBase::IntSCvT", __FILE__, __LINE__, eos,
                  "ODE failed with status: %s\n", ODE::ErrorStatus(status) );
        value = EOSbase::NaN;
    }
    return value;
}
