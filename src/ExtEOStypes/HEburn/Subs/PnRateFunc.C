#include <PTequilibrium.h>
#include <PnRate.h>

int PnRate::Rate(double V, double e, const double *z, double *zdot)
{
    double p = P(V,e,z[0]);
    if( std::isnan(p) )
        return 1;
    if( p>P_cutoff && z[0]<1.0 )
    {
        double lambda = max(0.,z[0]);
        zdot[0] = Rate(lambda,p);
    }
    else
        zdot[0] = 0.0;
    return 0;
}

int PnRate::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0.0;
    if( z[0]>=1. )
        return 0;                   // no constraint
    if( source_type != 2 )
    {
        double lambda  = z[0];
        double p = P(V,e,z[0]);
        if( std::isnan(p) )
            return -1;                  // error
        if( p < P_cutoff )
            return 0;                   // no constraint
        double rate = Rate(lambda,p);   // max rate
        dt = max(dlambda_dt/rate,dt_min);
    }
    return source_type;             // Integrate() available
}

int PnRate::Integrate(double V, double e, double *z, double dt)
{
    if( HE == NULL )
        return 1;
    if( dt <= 0.0 )
        return 0;
    if( z[0] >= lambda_burn )
    {
        z[0] = 1.;
        return 0;
    }
    double p = P(V,e,z[0]);
    if( std::isnan(p) )
        return 1;
    if( p < P_cutoff )
        return 0;
    V0 = V;
    e0 = e;
    if( integrate == 0 )
    {
        int status = step2(z[0],dt);
        if( status )
        {
            if( z[0]+dlambda >= 1. )
            {
                z[0] = 1.;
                return 0;
            }
            else
                return 1;
        }
        if( z[0] >= lambda_burn )
            z[0] = 1.;
        return 0;
    }
    int level = 0;
    min_level = max_level =0;
    count = 0;
    rate1 = Rate(z[0],p);
    double dt1 = 0.25*dlambda/rate1;
    while( dt > 0. )
    {
        count++;
        if( z[0]+dlambda >= 1. )
        {
            int status = step2(z[0],dt);
            if( status || z[0] >= lambda_burn )
                z[0] = 1.;
            return 0;
        }
        dt1 = min(dt,dt1);
        if( step1(z[0],dt1) )
            return 1;
        double eps = std::abs(y1-y2)/tol;       
        if( eps > 1. )
        {
            max_level=max(++level,max_level);
            dt1 *= 0.5;
            continue;
        }
        if( y2 >= lambda_burn )
        {
            z[0] = 1.;
            return 0;
        }
        z[0] = y2;
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
int PnRate::step1(double lambda, double dt)
{
    double p0 = P(V0,e0,lambda);
    if( std::isnan(p0) )
        return 1;
    double rate0  = Rate(lambda,p0);
    double lambda1 = min(1.,lambda+dt*rate0);
    double p1 = P(V0,e0,lambda1);
    if( std::isnan(p1) )
        return 1;
    y1 = 0.5*(lambda+lambda1+dt*Rate(lambda1,p1));  // second order
    //
    y2 = min(1.,lambda+0.5*dt*rate0);
    lambda1 = y2;
    p1 = P(V0,e0,lambda1);
    if( std::isnan(p1) )
        return 1;
    lambda1 = min(1.,lambda+dt*Rate(lambda1,p1));  
    p1= P(V0,e0,lambda1);
    if( std::isnan(p1) )
        return 1;
    rate1 = Rate(lambda1,p1);
    y2 = 0.5*(y2+lambda1+0.5*dt*rate1);             // second order
    y2 = min(1., (4.*y2-y1)/3.);                    // fourth order
    return 0;
}

// asymptotic for lambda ~ 1
int PnRate::step2(double &lambda, double dt)
{
    double p0 = P(V0,e0,lambda);
    if( std::isnan(p0) )
        return 1;   
    double kt = k*dt*pow(p0/Pcj,N);
    if( nu == 1. )
        // d(lambda)/dt = kp*(1-lambda)
        lambda = 1. - (1.-lambda)*exp(-kt);
    else if( nu == 0.0 )
        // d(lambda)/dt = kp
        lambda = min(1., lambda+kt);
    else
    {   // d(lambda)/dt = kp*(1-lambda)^nu, 0<nu<1
        double x = pow(1.-lambda, 1.-nu) - (1.-nu)*kt;
        lambda = (x>0.) ? max(lambda,1.-pow(x,1./(1.-nu))) : 1.;
    }
    return 0;
}

double PnRate::Dt(double V, double e, const double *z, double lambda)
{
    if( HE == NULL )
        return EOS::NaN;
    lambda = min(lambda,1.-dlambda);
    if( z[0] >= lambda )
        return 0.0;
    double p = P(V,e,z[0]);
    if( std::isnan(p) || p < P_cutoff )
        return EOS::NaN;
    double t = 0.0;
    double dt = dlambda/Rate(z[0],p);
    V0 = V;
    e0 = e;
    double lambda1 = z[0];
    while( lambda1 < lambda )
    {
        if( step1(lambda1,dt) )
            return EOS::NaN;
        double eps = std::abs(y2-y1)/tol;
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
