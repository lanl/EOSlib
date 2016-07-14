#include "PTequilibrium.h"
#include "FFQrate.h"

double FFQrate::Xpop(double p)
{
   double lnx = (a*log(10.) - log(p/Pref))/b;
   return xref*exp(lnx);
}

double FFQrate::Rate(double p)
{
   if( p < hug.ws0.P )
       return 0.0;
   p = min(p,hug.CJ.P);
   hug.P(p);
   double dP,dT;
   if( hug.ddlambda(hug.ws.V,hug.ws.e,hug.ws_lambda,dP,dT) )
       return EOS::NaN;
   double C = c(hug.ws.V,hug.ws.e,hug.ws_lambda);  
   double u  = hug.ws.u;
   double us = hug.ws.us;
   double rho = 1./hug.ws.V;
   double rho0 = 1./hug.ws0.V;
   double dPdx = b*(p/Xpop(p));
   double udu = (u/us)*hug.dUsdUp(u);
   return (1.+ (rho/rho0)*(C/us)/(1.+udu))*(us/dP)*dPdx;
   // For reactive shock
   // Forest Fire rate = Rate(p)/(1.-hug.ws_lambda)^n
}

double FFQrate::Rate(double lambda, double p)
{
   lambda = max(0.,min(1.,lambda));
   double rate;
   double w = 1. - lambda;
   if( p < Pmax)
   {
       rate  = Rate(p);
       w /= 1.-hug.ws_lambda;  // reactive shock
   }
   else
   {
       rate = Rmax;
       w /= 1.-lambda_max;
   }
   if( n == 1 )
       return w*rate;
   else if( n > 0. )
       return pow(w,n)*rate;
   else
       return rate;    
}


int FFQrate::Rate(double V, double e, const double *z, double *zdot)
{
    zdot[1] = 0.0;      // Q set by solver
    if( z[0] < 1.0 )
    {
        double p = P(V,e,z[0]);
        if( std::isnan(p) )
            return 1;
        zdot[0] = Rate(z[0],p+max(z[1],0.0));
    }
    else
        zdot[0] = 0.0;
    return 0;
}

int FFQrate::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0.0;
    if( z[0]<0. || 1.<=z[0] )
        return 0;       // no constraint
    double p = P(V,e,z[0]);
    if( std::isnan(p) )
        return -1;      // error
    double rate = Rate(z[0],p+max(z[1],0.0));
    if( rate == 0. )
        return 0;       // no constraint
    if( source_type != 2 )
        dt = max(dlambda_dt/rate,dt_min);
    return source_type; // Integrate() available
}

int FFQrate::Integrate(double V, double e, double *z, double dt)
{
    if( HE == NULL )
        return 1;
    if( dt<=0.0 || z[0]>=1. )
        return 0;
    double p = P(V,e,z[0]);
    if( std::isnan(p) )
        return 1;
    V0 = V;
    e0 = e;
    Q  = max(z[1],0.0);
    if( integrate == 0 )
        return step2(z[0],dt);
    int level = 0;
    min_level = max_level = 0;
    count = 0;
    rate1 = Rate(z[0],p+Q);
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
        double eps = std::abs(w1-w2)/tol;       
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
int FFQrate::step1(double lambda, double dt)
{
    double p = P(V0,e0,lambda);
    if( std::isnan(p) )
        return 1;
    double rate0  = Rate(lambda,p+Q);
    double lambda1 = min(1.,lambda+dt*rate0);
    p = P(V0,e0,lambda1);
    if( std::isnan(p) )
        return 1;
    w1 = 0.5*(lambda+lambda1+dt*Rate(lambda1,p+Q)); // second order
    //
    w2 = min(1.,lambda+0.5*dt*rate0);
    lambda1 = w2;
    p = P(V0,e0,lambda1);
    if( std::isnan(p) )
        return 1;
    lambda1 = min(1.,lambda+dt*Rate(lambda1,p+Q));  
    p= P(V0,e0,lambda1);
    if( std::isnan(p) )
        return 1;
    rate1 = Rate(lambda1,p+Q);
    w2 = 0.5*(w2+lambda1+0.5*dt*rate1);             // second order
    w2 = min(1., (4.*w2-w1)/3.);                    // fourth order
    return 0;
}

// asymptotic for lambda ~ 1
int FFQrate::step2(double &lambda, double dt)
{
    double p = P(V0,e0,lambda);
    if( std::isnan(p) )
        return 1;
    double rate, w;
    p += Q;
    if( p < Pmax)
    {
        rate = Rate(p);
           w = (1.-hug.ws_lambda);
    }
    else
    {
        rate = Rmax;
           w = (1.-lambda_max);
    }
    rate *= dt;
    if( n == 1. )
        // d(lambda)/dt = rate*(1-lambda)
        lambda = 1. - (1.-lambda)*exp(-rate/w);
    else if( n > 0.0 )
    {   // d(lambda)/dt = (rate/w^n)*(1-lambda)^n, 0<n<1
        double x = pow(1.-lambda, 1.-n) - (1.-n)*rate/pow(w,n);
        lambda = (x>0.) ? max(lambda,1.-pow(x,1./(1.-n))) : 1.;
    }
    else
        // d(lambda)/dt = rate
        lambda = min(1., lambda+rate);
    return 0;
}

double FFQrate::Dt(double V, double e, const double *z, double lambda)
{
    if( HE == NULL )
        return EOS::NaN;
    lambda = min(lambda,1.-dlambda);
    if( z[0] >= lambda )
        return 0.0;
    double p = P(V,e,z[0]);
    if( std::isnan(p) )
        return EOS::NaN;
    V0 = V;
    e0 = e;
    Q  = max(z[1],0.0);
    double t = 0.0;
    double dt = dlambda/Rate(z[0],p+Q);
    double lambda1 = z[0];
    while( lambda1 < lambda )
    {
        if( step1(lambda1,dt) )
            return EOS::NaN;
        double eps = std::abs(w2-w1)/tol;
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
