#include <PTequilibrium.h>
#include <HotSpotCC_Rate.h>
#include <LocalMath.h>

//#include <iostream>

//
// model specific functions
//
double HotSpotCC_Rate::P(double V, double e, double s, double s2)
{
    double lambda = g(s);
    e -= lambda*de(s2*s2);
    return HErate::P(V,e,lambda);
}
double HotSpotCC_Rate::T(double V, double e, double s, double s2)
{
    double lambda = g(s);
    e -= lambda*de(s2*s2);
    return HErate::T(V,e,lambda);
}
double HotSpotCC_Rate::c(double V, double e, double s, double s2)
{
    double lambda = g(s);
    e -= lambda*de(s2*s2);
    return HErate::c(V,e,lambda);
}

double HotSpotCC_Rate::g(double s)
{
    if( s <= 0.0 )
    {
        return 0.0;
    }
    else if( s < sc )
    {
        return gf*(1.-exp(-s*s));
    }
    else if( s < smax )
    {
        return gf*(gc+(s-sc)*(dgc+0.5*(s-sc)*d2gc));
    }
    else
        return 1.0;
}
double HotSpotCC_Rate::ginv(double lambda)
{
    if( lambda<=0.0 )
        return 0.0;
    if( lambda>=1.0 )
        return smax;
    double s;
    if( lambda < gf*gc )
    {   // lambda/gf = 1-exp(-s^2)
        s = sqrt(-log(1.-lambda/gf) );
    }
    else
    {   // lambda/gf - gc = dgc*(s-sc) + 0.5*d2gc*(s-sc)^2
        s = smax + sqrt(dgc*dgc + 2.*d2gc*(lambda/gf-gc))/d2gc;
    }
    return s;
}
double HotSpotCC_Rate::dgds(double s)
{
    if( s <= 0.0 )
    {
        return 0.0;
    }
    else if( s < sc )
    {
        return gf* 2.*s*exp(-s*s);
    }
    else if( s < smax )
    {
        return gf*(dgc+d2gc*(s-sc));
    }
    else
        return 0.0;
}

double HotSpotCC_Rate::f(double ps)
{
// ToDo: df/dp continuous at ps = Pign and ps = Pmax
    double ans;
    if( ps < Pign )
        ans = exp(A+B*ps/Pref);
    else if( ps < Pmax )
        ans = ((ps-Pign)*fmax+(Pmax-ps)*fign) / (Pmax-Pign);
    else
        ans = fmax;
    return ans/tref;
}

double HotSpotCC_Rate::dfdp(double ps)
{
// ToDo: continuous at ps = Pign and ps = Pmax
    double ans;
    if( ps < Pign )
        ans = (B/Pref)*exp(A+B*ps/Pref);
    else if( ps < Pmax )
        ans = (fmax-fign) / (Pmax-Pign);
    else
        ans = 0.0;
    return ans/tref;
}

double HotSpotCC_Rate::h(double t)
{
    if( t <= 0.0 )
        return 0.0;
    else if( t < t1 )
    {
    // Bezier curve
    // t = 2*(1-alpha)*alpha*(t1-h1/a2) + alpha^2 * t1
    //   = 2*(t1-h1/a2)*alpha + (2*h1/a2 -t1)*alpha^2
    // alpha = (-(t1-h1/a2) + sqrt[(t1-t-h1/a2)^2 + t*(t1-t)])/(2h1/a2 - t1)
    // or if t1 = 2*h1/a2, alpha = 0.5*t/(t1-h1/a2)
    // h = alpha^2 * h1
        double alpha;
        double dt1 = t1-h1/a2;
        double dt2 = 2.*h1/a2 - t1;
        if( abs(dt2) > 1e-4 * t1 )
            alpha = (-dt1 + sqrt(sqr(t1-t-h1/a2) + t*(t1-t))) / dt2;
        else
            alpha = 0.5*t/dt1 * (1.0-0.5*t*dt2/sqr(dt1));
        return alpha*alpha*h1;
    }
    else if( t < t2 )
        return h1 + a2*t;
    else if( t < t3 )
        return 1.0 - 0.5*a3*(t3-t)*(t3-t);
    else
        return 1.0;
}

double HotSpotCC_Rate::hinv(double lambda)
{
    if( lambda <= 0.0 )
        return 0.0;
    else if( lambda < h1 )
    {
    // Bezier curve
        double alpha2 = lambda/h1;
        double alpha = sqrt(alpha2);
        return 2.*(1-alpha)*alpha*(t1-h1/a2) + alpha2*t1;
    }
    else if( lambda < h2 )
        return t1 + (lambda-h1)/a2;
    else if( lambda < 1. )
        return t3 - sqrt(2.*(1.0-lambda)/a3);
    else
        return t3;   
}

double HotSpotCC_Rate::de(double lambda2)
{
    double ratio = 1. + lambda2*(Nratio-1.);     // N(lambda)/N0
    return (1./cbrt(ratio) - eratio)*Q;
}

double HotSpotCC_Rate::dde(double lambda2)
{   // (d/dlambda2) de
    double ratio = 1. + lambda2*(Nratio-1.);     // N(lambda)/N0
    return -((Nratio-1.)/3.)*Q / (ratio*cbrt(ratio));
}

double HotSpotCC_Rate::rate_s2(double lambda, double s2)
{
        double lambda2 = max(0.0,min(1.0,s2*s2));
        double rate;
        if( lambda2 < h1 )
        {   // Bezier curve
            double alpha = sqrt(lambda2/h1);
            //double dlambda = 2.*alpha*h1;
            double dt = 2.*(1.-alpha)*tb + 2.*alpha*(t1-tb);
            rate = 2.*sqrt(h1)/dt; 
        }
        else if( lambda2 <= h2 )
            rate = a2/s2;
        else if( lambda2 < 1. )
            rate = sqrt(2.*(1.-lambda2)*a3)/s2;

        return 0.5*lambda*lambda*rate;   // ds2/dt
}
//
//  HErate functions
//
int HotSpotCC_Rate::Rate(double V, double e, const double *z, double *zdot)
{
    double Ps = z[0];
    double ts = z[1];
    zdot[0] = zdot[1] = 0.0;
    zdot[2] = zdot[3] = 0.0;
    zdot[4] = zdot[5] = 0.0;
    if( Ps <= 0.0 || ts < 0.0 )
        return 0;
    double lambda = z[2];
    double s      = z[3];
    double s2     = z[4];
    if( s < smax )
    {
        double Dp = 1.0;
        if( n != 0 )
        {
            double p = P(V,e,s,s2);
            if( std::isnan(p) )
                return 1;
            Dp = D(p/Ps);
        }
        zdot[3] = f(Ps)*Dp;             // ds/dt
        zdot[2] = dgds(s)*zdot[3];      // dlambda/dt
    }
    if( s2 < 1. )
    {
        zdot[4] = rate_s2(lambda,s2);   // ds2/dt
        // zdot[5] = (d/dt) e_cc
        double lambda2 = s2*s2;
        zdot[5] = de(lambda2)*zdot[2] + lambda2*dde(lambda2)*(2.*s2*zdot[4]);
    }
    return 0;
}

int HotSpotCC_Rate::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0.0;
    if( source_type == 2 )
        return 2;               // stiff integrate, no time step constraint
    double Ps     = z[0];
    double ts     = z[1];
    if( Ps <= 0.0 || ts <= 0.0 )
        return 0;               // no constraint, not started to burn
    double s      = z[3];
    double s2     = z[4];
    if( s >= smax && s2 >= 1.  )
        return 0;               // no constraint, all burned
    double s_min = 0.02;        // make input parameter?
    double lambda = z[2];
    double rate;
    s = max(s_min, min(0.98*smax,s));   // s_min < s < 0.98*smax
    double Dp = 1.0;
    if( n != 0 )
    {
        double p = P(V,e,s,s2);
        if( std::isnan(p) )
            return -1;      // error
        Dp = D(p/Ps);
    }
    rate = dgds(s) * f(Ps)*Dp;  // dlambda/dt
    dt = dlambda/rate;
    double s2_max = 0.98;       // avoid rate=0 at s2=1, make input parameter?
    if( lambda>0.0 && s2 < s2_max )
    {
        rate = 2.*s2*rate_s2(lambda,s2);    // dlambda2/dt
        dt = min(dt, dlambda/rate);
    }
    return source_type;
}

int HotSpotCC_Rate::Integrate(double V, double e, double *z, double dt)
{
    if( HE == NULL )
        return 1;
    if( dt <= 0.0 )
        return 0;
    double Ps = z[0];
    double ts = z[1];
    if( Ps<=0.0 || ts<=0.0 )
        return 0;       // burning not yet started
    double lambda = z[2];
    double s      = z[3];
    double s2     = z[4];
    if( s >= smax && s2 >= 1. )
    {   // all burned
        z[2] = 1.0;     // lambda
        z[3] = smax;    // s
        z[4] = 1.0;     // s2
        z[5] = 0.0;     // e_cc
        return 0;
    }
    // first reaction
    if( s < smax )
    {
        double Dp = 1.0;
        double rate0 = f(Ps);
        if( n != 0 )
        {
            // predictor
            double p = P(V,e, s,s2);
            if( std::isnan(p) )
                return 1;
            Dp = D(p/Ps);
            // corrector
            double rate2 = rate_s2(s*s,s2);
            p = P(V,e, s+dt*Dp*rate0,s2+dt*rate2);
            if( std::isnan(p) )
                return 1;
            Dp = 0.5*(Dp+D(p/Ps));
        }
        s += dt*Dp*rate0;
    }
    if( s >= smax )
    {
        z[2] = 1.0;     // lambda
        z[3] = smax;    // s
    }
    else
    {
        z[2] = g(s);
        z[3] = s;
    }
    // second reaction
    lambda = z[2];
    double rate0 = rate_s2(lambda,s2);
    double rate1 = rate_s2(lambda,s2+dt*rate0);
    s2 += 0.5*dt*(rate0+rate1);
    if( s2 >= 1.0 )
    {
        z[4] = 1.0;     // s2
        z[5] = 0.0;     // e_cc
    }
    else
    {
        z[4] = s2;
        z[5] = lambda*de(s2*s2);
    }
    return 0;
}

double HotSpotCC_Rate::Dt(double V, double e, const double *z, double lambda)
{
//  return dt such that lambda = lambda(t+dt) where lambda(t) = z[2]
    if( HE == NULL )
        return EOS::NaN;
    double Ps = z[0];
    double ts = z[1];
    if( Ps<=0.0 || ts<=0.0 )
        return EOS::NaN;       // burning not yet started

    double lambda_s = z[2];
    double s        = z[3];
    if( lambda <= lambda_s )
        return 0.0;

    double s_t = ginv(lambda);
    double Dp = 1.0;
    double rate0 = f(Ps);
    if( n != 0 )
    {
        // predictor
        double s2 = z[4];
        double p = P(V,e,s,s2);
        if( std::isnan(p) )
            return EOS::NaN;
        Dp = D(p/Ps);
        double dt = (s_t - s)/(Dp*rate0);
        //second rate
        double rate2 = rate_s2(lambda,s2);
        s2 += rate2*dt;
        // corrector
        p = P(V,e,s_t,s2);
        if( std::isnan(p) )
            return EOS::NaN;
        Dp = 0.5*(Dp+D(p/Ps));
    }
    return (s_t - s)/(Dp*rate0);
}
