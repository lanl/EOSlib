#include <ArrheniusRate.h>
#include <iostream>
#include <iomanip>
using namespace std;

static double DLAMBDA = 0.01;        // default for dlambda

ArrheniusRate::ArrheniusRate(ExtEOS &Eos)
        : n(1.0), k(0.0),T_a(0.0),T_cutoff(0.0), dlambda(DLAMBDA)
{
    eos = Eos.Duplicate();
}

ArrheniusRate::ArrheniusRate(const ArrheniusRate &AR)
{
    eos      = AR.eos->Duplicate();
    n        = AR.n;
    k        = AR.k;
    T_a      = AR.T_a;
    T_cutoff = AR.T_cutoff;
    dlambda  = AR.dlambda;  
}

ArrheniusRate::~ArrheniusRate()
{
    deleteExtEOS(eos);
}

void ArrheniusRate::InitParams(Calc &calc)
{
    n = 1.; // first order
    k = T_a = 0.0;
    T_cutoff = -1.;
    dlambda = DLAMBDA;
    calc.Variable("n", &n);
    calc.Variable("k", &k);
    calc.Variable("T_a", &T_a);
    calc.Variable("T_cutoff", &T_cutoff);
    calc.Variable("dlambda",  &dlambda);
}

void ArrheniusRate::PrintParams(ostream &out) const
{
    out << "\tn   = " << n   << "\n"
        << "\tk   = " << k   << "\n"
        << "\tT_a = " << T_a << "\n"
        << "\tT_cutoff = " << T_cutoff << "\n"
        << "\t dlambda = " << dlambda  << "\n";
}

int ArrheniusRate::ConvertParams(Convert &convert)
{
    double s_t, s_T;
    if( !finite(s_t=convert.factor("time")) || !finite(s_T=convert.factor("T")) )
        return 1;
    k /= s_t;
    T_a *= s_T;
    T_cutoff *= s_T;
    return 0;    
}


static double RMIN = 1.0e-10;   // minimum rate for default T_cutoff
int ArrheniusRate::ParamsOK()
{
    if( n<0.0 || n>1. )
        return 1;
    if( k <= 0.0 )
        return 1;
    if( T_a <= 0.0 )
        return 1;
    if( T_cutoff < 0.0 )
        T_cutoff = max(-T_a/log(RMIN/k),0.0);
    if( dlambda<=0. || dlambda>=1. )
        return 1;
    return 0;
}

int ArrheniusRate::Rate(double V, double e, const double *z, double *zdot)
{
    double T = eos->T(V,e,z);
    if( std::isnan(T) )
        return 1;
    if( T>T_cutoff && z[0]<1.0 )
    {
        double lambda = max(0.,z[0]);
        zdot[0] = Rate(lambda,T);
    }
    else
        zdot[0] = 0.0;
    return 0;
}

#define DLAMBDA 0.1
int ArrheniusRate::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0.0;
    if( z[0]>=1. )
        return 0;
    double lambda  = z[0];
    double lambda1 = min(1.0,lambda+DLAMBDA);
    double ztmp[1] = {lambda1};
    double T = eos->T(V,e,ztmp);
    if( std::isnan(T) )
        return 1;
    if( T < T_cutoff )
        return 0;
    double rate = Rate(lambda,T);   // max rate
    dt = DLAMBDA/rate;
    return 0;
}

int ArrheniusRate::Integrate(double V, double e, double *z, double dt)
{
    if( eos == NULL )
        return 1;
    if( dt <= 0.0 || z[0] >= 1. )
        return 0;
    double T = eos->T(V,e,z);
    if( std::isnan(T) )
        return 1;
    if( T < T_cutoff )
        return 0;
    double dt1 = dlambda/Rate(z[0],T);
    V0 = V;
    e0 = e;
    while( dt > 0. )
    {
        if( z[0]+dlambda >= 1. )
            return step2(z,dt);
        dt1 = min(dt,dt1);
        if( step1(z,dt1) )
            return 1;
        double eps = abs(z2[0]-z3[0])/1.e-4;       
        if( eps > 1. )
        {
            dt1 *= 0.5;
            continue;
        }
        dt -= dt1;
        z[0] = 0.5*(z2[0]+z3[0]);
        if( eps < 0.25 )
            dt1 *= 2.;
        if( z[0] >= 1.0 )
            return 0;
    }
    return 0;
}


int ArrheniusRate::step1(double *z, double dt)
{
    double T0 = eos->T(V0,e0,z);
    if( std::isnan(T0) )
        return 1;
    ddt0  = Rate(z[0],T0);
    // half step
    z1[0] = min(1.,z[0]+0.5*dt*ddt0);
    double T1 = eos->T(V0,e0,z1);
    if( std::isnan(T1) )
        return 1;
    // full step using midpoint integration
    z2[0] = min(1.,z[0]+dt*Rate(z1[0],T1));
    double T2 = eos->T(V0,e0,z2);
    if( std::isnan(T2) )
        return 1;
    ddt2 = Rate(z2[0],T2);
    // full step using trapezoidal integration
    z3[0] = min(1.,z[0]+0.5*dt*(ddt0+ddt2));
    return 0;
}

// asymptotic for lambda ~ 1
int ArrheniusRate::step2(double *z, double dt)
{
    double T0 = eos->T(V0,e0,z);
    if( std::isnan(T0) )
        return 1;   
    double kt = k*dt*exp(-T_a/T0);
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
    return 0;
}


double ArrheniusRate::Dt(double V, double e, const double *z, double lambda)
{
    if( eos == NULL )
        return EOS::NaN;
    lambda = min(lambda,1.-dlambda);
    if( z[0] >= lambda )
        return 0.0;
    double t = 0.0;
    double T = eos->T(V,e,z);
    if( std::isnan(T) || T < T_cutoff )
        return EOS::NaN;
    double dt = dlambda/Rate(z[0],T);
    V0 = V;
    e0 = e;
    z0[0] = z[0];
    while( z0[0] < lambda )
    {
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
        if( eps < 0.25 )
            dt *= 2.;         
    }
    return t;
}
