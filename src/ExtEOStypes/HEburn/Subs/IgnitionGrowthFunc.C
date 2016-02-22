#include <PTequilibrium.h>
#include <IgnitionGrowth.h>

double IgnitionGrowth::Rate(double lambda, double V, double p)
{
    double rate = 0.0;
    if( lambda < lambda_igmax && 1./(V*rho0)>1.+a )
        rate = I*pow(1.-lambda,b)*pow(1./(V*rho0)-1.-a,x); 
    if( lambda < lambda_G1max )
        rate += G1*pow(1.-lambda,c)*pow(lambda,d)*pow(p,y);
    if( lambda_G2min <= lambda)
        rate += G2*pow(1.-lambda,e)*pow(lambda,g)*pow(p,zp);
    return rate;
}


int IgnitionGrowth::Rate(double V, double e, const double *z, double *zdot)
{
    if( z[0] < 0.0 || (z[0]==0 && 1.<(V*rho0)*(1.+a)) || 1.<=z[0] )
        zdot[0] = 0.0;
    else
    {
        double p = P(V,e,z[0]);
        if( isnan(p) )
            return 1;
        zdot[0] = Rate(z[0],V,p);
    }
    return 0;
}

int IgnitionGrowth::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0.0;
    if( z[0]<0.|| 1.<=z[0] )
        return 0;
    double p = P(V,e,z[0]);
    if( isnan(p) )
        return -1;
    double rate = Rate(z[0],V,p);
    if( rate == 0. )
        return 0;
    dt = dlambda_dt/rate;
    return 3;       // Integrate() available
}

int IgnitionGrowth::Integrate(double V, double e, double *z, double dt)
{
    int level = 0;
    min_level = max_level = 0;
    count = 0;
    if( HE == NULL )
        return 1;
    if( dt<=0.0 || z[0]>=1. )
        return 0;
    double p = P(V,e,z[0]);
    if( isnan(p) )
        return 1;
    V0 = V;
    e0 = e;
    rate1 = Rate(z[0],V0,p);
    if( rate1 == 0. )
        return 0;
    double dt1 = 0.25*dlambda/rate1;
    while( dt > 0. )
    {
        count++;
        if( z[0]+dlambda >= 1. )
            return step2(z[0],dt);
        dt1 = min(dt,dt1);
        if( step1(z[0],dt1) )
            return 1;
        double eps = abs(w1-w2)/tol;       
        if( eps > 1. )
        {
            max_level=max(++level,max_level);
            dt1 *= 0.5;
            continue;
        }
        z[0] = w2;
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
int IgnitionGrowth::step1(double lambda, double dt)
{
    double P0 = P(V0,e0,lambda);
    if( isnan(P0) )
        return 1;
    double rate0  = Rate(lambda,V0,P0);
    double lambda1 = min(1.,lambda+dt*rate0);
    double P1 = P(V0,e0,lambda1);
    if( isnan(P1) )
        return 1;
    w1 = 0.5*(lambda+lambda1+dt*Rate(lambda1,V0,P1));    // second order
    //
    w2 = min(1.,lambda+0.5*dt*rate0);
    lambda1 = w2;
    P1 = P(V0,e0,lambda1);
    if( isnan(P1) )
        return 1;
    lambda1 = min(1.,lambda+dt*Rate(lambda1,V0,P1));  
    P1= P(V0,e0,lambda1);
    if( isnan(P1) )
        return 1;
    rate1 = Rate(lambda1,V0,P1);
    w2 = 0.5*(w2+lambda1+0.5*dt*rate1);           // second order
    w2 = min(1., (4.*w2-w1)/3.);                // fourth order
    return 0;
}

// asymptotic for lambda ~ 1
int IgnitionGrowth::step2(double &lambda, double dt)
{
    double p = P(V0,e0,lambda);
    if( isnan(p) )
        return 1;   
    double kt = G2*pow(lambda,g)*pow(p,zp)*dt;
    if( e == 1. )
        // d(lambda)/dt = kp*(1-lambda)
        lambda = 1. - (1.-lambda)*exp(-kt);
    else if( e == 0.0 )
        // d(lambda)/dt = kp
        lambda = min(1., lambda+kt);
    else
    {   // d(lambda)/dt = kp*(1-lambda)^e, 0<e<1
        double q = pow(1.-lambda, 1.-e) - (1.-e)*kt;
        lambda = (q>0.) ? max(lambda,1.-pow(q,1./(1.-e))) : 1.;
    }
    return 0;
}


double IgnitionGrowth::Dt(double V, double e, const double *z, double lambda)
{
    if( HE == NULL )
        return EOS::NaN;
    lambda = min(lambda,1.-dlambda);
    if( z[0] >= lambda )
        return 0.0;
    double p = P(V,e,z[0]);
    if( isnan(p) )
        return EOS::NaN;
    double t = 0.0;
    double dt = dlambda/Rate(z[0],V,p);
    V0 = V;
    e0 = e;
    double lambda1 = z[0];
    while( lambda1 < lambda )
    {
        if( step1(lambda1,dt) )
            return EOS::NaN;
        double eps = abs(w2-w1)/tol;
        if( eps > 1. )
        {
            dt *= 0.5;
            continue;
        }
        t += dt;
        lambda1 = w2;
        if( eps < 0.125 )
            dt *= 2.;         
    }
    return t;
}
