#include <cmath>
#include <PTequilibrium.h>
#include <HotSpotRateExp.h>

double HotSpotRateExp::s0(double Ps, double Ts)
{
    double Ths = Ts + a*(Ps/Pref);
    return exp(min(b+Ths/Tw,lns0_max)) / tref;
}

int HotSpotRateExp::Rate(double V, double e, const double *z, double *zdot)
{
    double     Ps = z[0];
    double     Ts = z[1];
    double     ts = z[2];
    double lambda = z[3];
    zdot[0] = zdot[1] = zdot[2] = zdot[5] = 0.0;
    if( lambda < 1. && Ps > 0.0 && ts >= 0.0 )
    {
        double p = P(V,e,lambda);
        if( std::isnan(p) )
            return 1;
        double s  = z[4];
        double ds = (z[5]>0.0) ? z[5] : s0(Ps,Ts);
        zdot[4] = ds*D(p/Ps);           // ds/dt
        zdot[3] = dgds(s)*zdot[4];      // dlambda/dt
    }
    else
    {
        zdot[3] = 0.0;
        zdot[4] = 0.0;
    }
    return 0;
}

int HotSpotRateExp::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0.0;
    if( source_type == 2 )
        return 2;       // stiff integrate with no time step constraint
    double     Ps = z[0];
    double     Ts = z[1];
    double     ts = z[2];
    double lambda = z[3];
    double      s = z[4];
    if( Ps <= 0.0 || ts <= 0.0 || s > smax || lambda > lambda_burn )
        return 0;               // no constraint
    double smin = 0.01;         // make input parameter
    if( source_type != 2 )
    {
        double p = P(V,e,lambda);
        if( std::isnan(p) )
            return -1;                          // error
        double ds = (z[5]>0.0) ? z[5] : s0(Ps,Ts);
        double rate = ds*D(p/Ps)*dgds(max(s,smin));   // dlambda/dt
        dt = dlambda/rate;
    } 
    return source_type;
}

int HotSpotRateExp::Integrate(double V, double e, double *z, double dt)
{
    if( HE == NULL )
        return 1;
    if( dt <= 0.0 )
        return 0;
    double Ps = z[0];
    double ts = z[2];
    if( Ps<=0.0 || ts <= 0.0 )
        return 0;
    double lambda = z[3];
    double s      = z[4];
    if( lambda >= lambda_burn || s >= smax )
    {
        z[3] = 1.;
        z[4] = max(z[4],smax);
        return 0;
    }
    double p = P(V,e,lambda);
    if( std::isnan(p) )
        return 1;
    //if( z[5] <= 0.0 )
    {
        double Ts = z[1];
        z[5] = s0(Ps,Ts);
    }
    s += z[5]*D(p/Ps)*dt;
    if( s < smax )
    {      
        z[3] = g(s);
        z[4] = s;
    }
    else
    {
        z[3] = 1.0;
        z[4] = smax;
    }
    return 0;
}

double HotSpotRateExp::Dt(double V, double e, const double *z, double lambda)
{
//  return dt such that lambda = lambda(t+dt,Ps) where lambda(t,Ps) = z[2]
    if( HE == NULL )
        return EOS::NaN;
    if( lambda <= 0.0 )
        return 0.0;
    lambda = min(lambda,lambda_burn);
    if( z[3] >= lambda )
        return 0.0;
    double Ps = z[0];
    double ts = z[2];
    if( Ps<=0.0 || ts<=0.0 )
        return EOS::NaN;
    
    double s1 = z[4];
    if( g(s1) >= lambda )
        return 0.0;

    double p0 = P(V,e,z[3]);
    if( std::isnan(p0) )
        return EOS::NaN;
    double p1 = P(V,e,lambda);
    if( std::isnan(p1) )
        return EOS::NaN;
    double Dp = 0.5*(D(p0/Ps)+D(p1/Ps));  // approximate average D(p/Ps)
    double dsdt = (z[5]>0.0) ? z[5] : s0(Ps,z[1]);
    dsdt *= Dp;

    double s;
    s = sqrt(-log(1.-lambda));      // special case, lambda = 1 - exp(-s*s)
/************    
    double s2 = smax;
    int i;
    for( i=20; i>0; --i )
    {
        s = 0.5*(s1+s2);
        double lambda_s = g(s);
        if( lambda_s > lambda )
        {
            if( lambda_s-lambda < tol )
                break;
            s2 = s;
        }
        else
        {
            if( lambda-lambda_s < tol )
                return 0;
            s1 = s;
        }
    }
*********/
    return (s - z[4])/dsdt;
}
