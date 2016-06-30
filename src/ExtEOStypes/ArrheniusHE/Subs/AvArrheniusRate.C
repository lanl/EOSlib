#include <AvArrheniusRate.h>
#include <iostream>
#include <iomanip>
using namespace std;

static double DLAMBDA = 0.1;        // default for dlambda

AvArrheniusRate::AvArrheniusRate(ExtEOS &Eos)
        : n(0.0), k(0.0),T_a(0.0),T_cutoff(0.0),tau(0.0), dlambda(DLAMBDA)
{
    eos = Eos.Duplicate();
}

AvArrheniusRate::AvArrheniusRate(const AvArrheniusRate &AAR)
{
    eos      = AAR.eos->Duplicate();
    n        = AAR.n;
    k        = AAR.k;
    T_a      = AAR.T_a;
    T_cutoff = AAR.T_cutoff;
    tau      = AAR.tau;  
    dlambda  = AAR.dlambda;  
}

AvArrheniusRate::~AvArrheniusRate()
{
    deleteExtEOS(eos);
}

void AvArrheniusRate::InitParams(Calc &calc)
{
    n = k = T_a = tau = 0.0;
    T_cutoff = -1.;
    dlambda = DLAMBDA;
    calc.Variable("n", &n);
    calc.Variable("k", &k);
    calc.Variable("T_a", &T_a);
    calc.Variable("tau", &tau);
    calc.Variable("T_cutoff", &T_cutoff);
    calc.Variable("dlambda",  &dlambda);
}

void AvArrheniusRate::PrintParams(ostream &out) const
{
    out << "\tn   = " << n   << "\n"
        << "\tk   = " << k   << "\n"
        << "\tT_a = " << T_a << "\n"
        << "\ttau = " << tau << "\n"
        << "\tT_cutoff = " << T_cutoff << "\n"
        << "\t dlambda = " << dlambda  << "\n";
}

int AvArrheniusRate::ConvertParams(Convert &convert)
{
    double s_t, s_T;
    if( !finite(s_t=convert.factor("time")) || !finite(s_T=convert.factor("T")) )
        return 1;
    k /= s_t;
    T_a *= s_T;
    T_cutoff *= s_T;
    tau *= s_t;
    return 0;    
}


static double RMIN = 1.0e-10;
int AvArrheniusRate::ParamsOK()
{
    if( n<0.0 || n>1. )
        return 1;
    if( k <= 0.0 )
        return 1;
    if( T_a <= 0.0 )
        return 1;
    if( tau <= 0.0 )
        return 1;
    if( T_cutoff < 0.0 )
        T_cutoff = max(-T_a/log(RMIN/k),0.0);
    if( dlambda<=0. || dlambda>=1. )
        return 1;
    return 0;
}

int AvArrheniusRate::Rate(double V, double e, const double *z, double *zdot)
{
    double T = eos->T(V,e,z);
    if( std::isnan(T) )
        return 1;
    double Tav = z[1];
    if( T>T_cutoff && z[0]<1.0 )
    {
        double lambda = max(0.,z[0]);
        zdot[0] = Rate(lambda,Tav);
    }
    else
        zdot[0] = 0.0;
    zdot[1] = (T-Tav)/tau;
    return 0;
}

#define DLAMBDA 0.1
int AvArrheniusRate::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0.0;
    if( z[0]>=1. )
        return 0;
    double lambda = z[0];
    double Tav    = z[1];    
    double lambda1 = min(1.0,lambda+DLAMBDA);
    double ztmp[2] = {lambda1,Tav};
    double T = eos->T(V,e,ztmp);
    if( std::isnan(T) )
        return 1;
    T = max(T,Tav);
    if( T < T_cutoff )
        return 0;
    double rate = Rate(lambda,T);   // max rate
    dt = max(tau,DLAMBDA/rate);
    return 0;
}
//
//  Tav(t) = Tav(0)*exp(-t/tau) + \int_0^t (dt'/tau)T(t')*exp(-(t-t')/tau)
//         = Tav(0) + \int_0^t (dt'/tau)[T(t')-Tav(0)]*exp(-(t-t')/tau)
//  For T(t) = T(0) +dT/dt(0)*t
//  Tav(t) = T(0) + [Tav(0)-T(0)]*exp(-t/tau)
//                + dT/dt(0)*[t + tau*(exp(-t/tau)-1)]
//  dT/dt = dT/dlambda * dlambda/dt
//        = dT/dlambda * Rate
//
int AvArrheniusRate::Integrate(double V, double e, double *z, double dt)
{
    if( eos == NULL )
        return 1;
    if( dt <= 0.0 || z[0] >= 1. )
        return 0;
    V0 = V;
    e0 = e;
    if( z[1] < T_cutoff )
        return step0(z,dt);       
    double dt1 = max(dlambda/Rate(z[0],z[1]), 0.1*tau);
    while( dt > 0. )
    {
        if( z[1] < T_cutoff )
            return step0(z,dt);
        if( dt < 0.1*tau || z[0]+dlambda > 1. )
            return step2(z,dt);
        dt1 = min(dt,dt1);
        if( step1(z,dt1) )
            return 1;
        double eps = abs(z2[0]-z3[0])/1.e-4 + abs(z2[1]-z3[1])/(1.e-3*z2[1]);       
        if( eps > 1. )
        {
            dt1 *= 0.5;
            continue;
        }
        dt -= dt1;
        z[0] = 0.5*(z2[0]+z3[0]);
        z[1] = 0.5*(z2[1]+z3[1]);
        if( eps < 0.25 )
            dt1 *= 2.;
        if( z[0] >= 1.0 )
            return 0;
    }
    return 0;
}

int AvArrheniusRate::step0(double *z, double dt)
{
    double T = eos->T(V0,e0,z);
    if( std::isnan(T) )
        return 1;
    z[1] = T - (T-z[1])*exp(-dt/tau);
    return 0;      
}

int AvArrheniusRate::step1(double *z, double dt)
{
    double T0 = eos->T(V0,e0,z);
    if( std::isnan(T0) )
        return 1;
    double exp_dt = exp(-0.5*dt/tau);        
    ddt0[1] = (T0-z[1])/tau;
      z1[1] = T0 - (T0-z[1])*exp_dt;
    ddt0[0] = Rate(z[0],z[1]);
      z1[0] = min(1.,z[0]+0.5*dt*ddt0[0]);
    double T1 = eos->T(V0,e0,z1);
    if( std::isnan(T1) )
        return 1;
    z1[1] += (T1-T0)*(1.+0.5*(tau/dt)*(exp_dt-1.)); 
    //
    exp_dt *= exp_dt;
    z2[0] = min(1.,z[0]+dt*Rate(z1[0],z1[1]));
    z2[1] = T0 - (T0-z[1])*exp_dt; 
    double T2 = eos->T(V0,e0,z2);
    if( std::isnan(T2) )
        return 1;
    z2[1] += (T2-T0)*(1.+(tau/dt)*(exp_dt-1.)); 
    ddt2[0] = Rate(z2[0],z2[1]);
    ddt2[1] = (T2-z2[1])/tau;
    //
    z3[0] = min(1.,z[0]+0.5*dt*(ddt0[0]+ddt2[0]));
    z3[1] = T0 - (T0-z[1])*exp_dt;   
    double T3 = eos->T(V0,e0,z3);
    if( std::isnan(T3) )
        return 1;
    z3[1] += (T3-T0)*(1.+(tau/dt)*(exp_dt-1.));   
    return 0;
}

// asymptotic for dt ~ tau or lambda ~ 1
int AvArrheniusRate::step2(double *z, double dt)
{
    // d(Tav)/dt = (T-Tav)/tau and T = const
    double T0 = eos->T(V0,e0,z);
    if( std::isnan(T0) )
        return 1;   
    double exp_dt = exp(-dt/tau);
    z[1] = T0 - (T0-z[1])*exp_dt;
    double kt = k*dt*exp(-T_a/z[1]);
    if( n == 1. )
        // d(lambda)/dt = kp*(1-lambda)
        z[0] = 1. - (1.-z[0])*exp(-kt);
    else if( n == 0.0 )
        // d(lambda)/dt = kp
        z[0] = min(1., z[0]+kt);
    else
    {   // d(lambda)/dt = kp*(1-lambda)^n, 0<n<1
        double x = pow(1.-z[0], 1.-n) - (1.-n)*kt;
        z[0] = (x>0.) ? max(z[0],1.-pow(x,1./(1.-n))) : 1.;
    }
    double T1 = eos->T(V0,e0,z);
    if( std::isnan(T1) )
        return 1;
    z[1] += (T1-T0)*(1. + (tau/dt)*(exp_dt-1.)); 
    return 0;
}


double AvArrheniusRate::Dt(double V, double e, const double *z, double lambda)
{
    if( eos == NULL )
        return EOS::NaN;
    if(  z[0] >= lambda )
        return 0.0;
    double t = 0.0;
    lambda = min(lambda,1.-dlambda);
    V0 = V;
    e0 = e;
    z0[0] = z[0];
    z0[1] = z[1];
    if( z0[1] < T_cutoff )
    {
        z0[1] = eos->T(V,e,z);
        if( std::isnan(z0[1]) || z0[1] < T_cutoff )
            return EOS::NaN;
        t = tau;
    }
    double dt = dlambda/Rate(z0[0],z0[1]);
    while( z0[0] < lambda )
    {
        if( z0[1] < T_cutoff )
            return EOS::NaN;
        if( step1(z0,dt) )
            return EOS::NaN;
        double eps = abs(z2[0]-z3[0])/1.e-4;
        if( eps > 1. )
        {
            dt *= 0.5;
            continue;
        }
        t += dt;
        z0[0] = 0.5*(z2[0]+z3[0]);
        z0[1] = 0.5*(z2[1]+z3[1]);
        if( eps < 0.25 )
            dt *= 2.;         
    }
    return t;
}
