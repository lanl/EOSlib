#include <PTequilibrium.h>
#include <HotSpotRate.h>

double HotSpotRate::f(double ps)
{
    if( ps < P1 )
    {
        if( P0 < 0.0 )  // backwards compatibility
            return exp(A+B*ps/Pref);
        if( ps <= P0 )
            return 0.0;
        return exp(A+B*ps/Pref) - fp0*(1.+B*(ps-P0)/Pref);
    }
    if( df1 <= 0. )
        return f1;
    return f1*(1.0 + df1*(1. - exp(-B2*(ps-P1)/Pref)));
}

double HotSpotRate::dfdp(double ps)
{
    if( ps <= P1 )
    {
        if( P0 < 0.0 )  // backwards compatibility
            return (B/Pref)*exp(A+B*ps/Pref);
        if( ps <= P0 )
            return 0.0;
        return (B/Pref)*(exp(A+B*ps/Pref) - fp0);
    }
    if( df1 <= 0. )
        return 0.0;
    return (B2/Pref)*f1*df1*exp(-B2*(ps-P1)/Pref);
}

int HotSpotRate::Rate(double V, double e, const double *z, double *zdot)
{
    double Ps = z[0];
    double ts = z[1];
    double lambda = z[2];
    zdot[0] = zdot[1] = 0.0;
    if( lambda < 1. && Ps > 0.0 && ts >= 0.0 )
    {
        double p = P(V,e,lambda);
        if( isnan(p) )
            return 1;
        double s  = z[3];
        zdot[3] = f(Ps)*D(p/Ps)/tref;      // ds/dt
        zdot[2] = dgds(s)*zdot[3];      // dlambda/dt
    }
    else
    {
        zdot[2] = 0.0;
        zdot[3] = 0.0;
    }
    return 0;
}

int HotSpotRate::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0.0;
    if( source_type == 2 )
        return 2;       // stiff integrate with no time step constraint
    double Ps = z[0];
    double ts = z[1];
    double lambda = z[2];
    double s      = z[3];
    if( Ps <= 0.0 || ts <= 0.0 || s > smax || lambda > lambda_burn )
        return 0;               // no constraint
    double smin = 0.01;         // make input parameter
    if( source_type != 2 )
    {
        double p = P(V,e,lambda);
        if( isnan(p) )
            return -1;                          // error
        double r0   = f(Ps);
        double dsdt = f(Ps)*D(p/Ps)/tref;
        double rate = dgds(max(s,smin))*dsdt;   // dlambda/dt
        dt = dlambda/rate;
    } 
    return source_type;
}

int HotSpotRate::Integrate(double V, double e, double *z, double dt)
{
    if( HE == NULL )
        return 1;
    if( dt <= 0.0 )
        return 0;
    double Ps = z[0];
    double ts = z[1];
    if( Ps<=0.0 || ts <= 0.0 )
        return 0;
    double lambda = z[2];
    double s      = z[3];
    if( lambda >= lambda_burn || s >= smax )
    {
        z[2] = 1.;
        z[3] = max(z[3],smax);
        return 0;
    }
    double p = P(V,e,lambda);
    if( isnan(p) )
        return 1;
    s += f(Ps)*D(p/Ps)*dt/tref;
    if( s < smax )
    {      
        z[2] = g(s);
        z[3] = s;
    }
    else
    {
        z[2] = 1.0;
        z[3] = smax;
    }
    return 0;
}

double HotSpotRate::Dt(double V, double e, const double *z, double lambda)
{
//  return dt such that lambda = lambda(t+dt,Ps) where lambda(t,Ps) = z[2]
    if( HE == NULL )
        return EOS::NaN;
    if( lambda <= 0.0 )
        return 0.0;
    lambda = min(lambda,lambda_burn);
    if( z[2] >= lambda )
        return 0.0;
    double Ps = z[0];
    double ts = z[1];
    if( Ps<=0.0 || ts<=0.0 )
        return EOS::NaN;
    

    double s1 = z[3];
    if( g(s1) >= lambda )
        return 0.0;

    double p0 = P(V,e,z[2]);
    if( isnan(p0) )
        return EOS::NaN;
    double p1 = P(V,e,lambda);
    if( isnan(p1) )
        return EOS::NaN;
    double Dp = 0.5*(D(p0/Ps)+D(p1/Ps));  // approximate average D(p/Ps)

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
    return tref*(s - z[3])/(f(Ps)*Dp);
}
