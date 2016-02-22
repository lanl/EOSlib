#include "PTequilibrium.h"
#include "Arrhenius.h"

int Arrhenius::Rate(double V, double e, const double *z, double *zdot)
{
    double Tz = T(V,e,z[0]);
    if( isnan(Tz) )
        return 1;
    if( Tz>T_cutoff && z[0]<1.0 )
    {
        double lambda = max(0.,z[0]);
        zdot[0] = Rate(lambda,Tz);
    }
    else
        zdot[0] = 0.0;
    return 0;
}

int Arrhenius::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0.0;
    if( z[0]>=1. )
        return 0;                       // no constraint
    double lambda  = z[0];
    double lambda1 = min(1.0,lambda+dlambda_dt);
    double T1 = T(V,e,lambda1);
    if( isnan(T1) )
        return -1;                      // error
    if( T1 < T_cutoff )
        return 0;                       // no constraint
    else if( source_type != 2 )
    {
        double rate = Rate(lambda,T1);  // max rate
        dt = max(dlambda_dt/rate,dt_min);
    }
    return source_type;
}


int Arrhenius::Integrate(double V, double e, double *z, double dt)
{
    if( HE == NULL )
        return 1;
    if( dt <= 0.0 || z[0] >= 1. )
        return 0;
    double T0 = T(V,e,z[0]);
    if( isnan(T0) )
        return 1;
    if( T0 < T_cutoff )
        return 0;
    V0 = V;
    e0 = e;
    if( integrate == 0 )
        return step2(z[0],dt);            
    int level = 0;
    min_level = max_level =0;
    count = 0;
    rate1 = Rate(z[0],T0);
    double dt1 = 0.25*dlambda/rate1;
    while( dt > 0. )
    {
        count++;
        if( z[0]+dlambda >= 1. )
            return step2(z[0],dt);
        dt1 = min(dt,dt1);
        if( step1(z[0],dt1) )
            return 1;
        double eps = abs(y1-y2)/tol;       
        if( eps > 1. )
        {
            max_level=max(++level,max_level);
            dt1 *= 0.5;
            continue;
        }
        z[0] = y2;
        if( z[0] >= 1.0 )
            return 0;
        dt -= dt1;
        if( eps < 0.125 )
        {
            min_level = min(--level,min_level);
            dt1 *= 2.;
        }
        else if( eps > 0.5 )
            dt1 = min(dt1,dlambda/rate1);
    }
    return 0;
}

// Modified Midpoint: Numerical recipies section 15.3
int Arrhenius::step1(double lambda, double dt)
{
    double T0 = T(V0,e0,lambda);
    if( isnan(T0) )
        return 1;
    double rate0  = Rate(lambda,T0);
    double lambda1 = min(1.,lambda+dt*rate0);
    double T1 = T(V0,e0,lambda1);
    if( isnan(T1) )
        return 1;
    y1 = 0.5*(lambda+lambda1+dt*Rate(lambda1,T1));  // second order
    //
    y2 = min(1.,lambda+0.5*dt*rate0);
    lambda1 = y2;
    T1 = T(V0,e0,lambda1);
    if( isnan(T1) )
        return 1;
    lambda1 = min(1.,lambda+dt*Rate(lambda1,T1));  
    T1= T(V0,e0,lambda1);
    if( isnan(T1) )
        return 1;
    rate1 = Rate(lambda1,T1);
    y2 = 0.5*(y2+lambda1+0.5*dt*rate1);             // second order
    y2 = min(1., (4.*y2-y1)/3.);                    // fourth order
    return 0;
}

// asymptotic for lambda ~ 1
int Arrhenius::step2(double &lambda, double dt)
{
    double T0 = T(V0,e0,lambda);
    if( isnan(T0) )
        return 1;   
    double kt = dt*Rate(T0);
    if( n == 1. )
        // d(lambda)/dt = kp*(1-lambda)
        lambda = 1. - (1.-lambda)*exp(-kt);
    else if( n == 0.0 )
        // d(lambda)/dt = kp
        lambda = min(1., lambda+kt);
    else
    {   // d(lambda)/dt = kp*(1-lambda)^n, 0<n<1
        double x = pow(1.-lambda, 1.-n) - (1.-n)*kt;
        lambda = (x>0.) ? max(lambda,1.-pow(x,1./(1.-n))) : 1.;
    }
    return 0;
}


double Arrhenius::Dt(double V, double e, const double *z, double lambda)
{
    if( HE == NULL )
        return EOS::NaN;
    lambda = min(lambda,1.-dlambda);
    if( z[0] >= lambda )
        return 0.0;
    double Tz = T(V,e,z[0]);
    if( isnan(Tz) || Tz < T_cutoff )
        return EOS::NaN;
    double t = 0.0;
    double dt = dlambda/Rate(z[0],Tz);
    V0 = V;
    e0 = e;
    double lambda1 = z[0];
    while( lambda1 < lambda )
    {
        if( step1(lambda1,dt) )
            return EOS::NaN;
        double eps = abs(y2-y1)/tol;
        if( eps > 1. )
        {
            dt *= 0.5;
            continue;
        }
        t += dt;
        lambda1 = y2;
        if( eps < 0.125 )
            dt *= 2.;         
    }
    return t;
}
