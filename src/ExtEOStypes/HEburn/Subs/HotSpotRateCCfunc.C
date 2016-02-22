#include <PTequilibrium.h>
#include <HotSpotRateCC.h>
#include <LocalMath.h>

//#include <iostream>

//
// model specific functions
//
int HotSpotRateCC::set_z(const char *var, double value, double *z)
{
    if( !strcmp(var,"lambda") )
    {
        z[2] = ginv(value);
        return 0;
    }
    if( !strcmp(var,"lambda2") )
    {
        value = max(0.0, min(1.0,value));
        z[3] = sqrt(value);
        return 0;
    }
    return 1;
}

int HotSpotRateCC::get_z(double *z, const char *var, double &value)
{
    if( !strcmp(var,"lambda") )
    {
        value = g(z[2]);
        return 0;
    }
    if( !strcmp(var,"lambda2") )
    {
        value = z[3]*z[3];
        return 0;
    }
    if( !strcmp(var,"e_cc") )
    {
        double lambda  = g(z[2]);
        double lambda2 = z[3]*z[3];
        value = q(lambda,lambda2);
        return 0;
    }
    return 1;
}

int HotSpotRateCC::dlambdadt(double V, double e, const double *z,
                             double &dlambda, double &dlambda2)
{
        double s = z[2];
        dlambda  = dgds(s)*rate_s(V,e,z);

        double lambda  = g(s);
        double s2 = z[3];
        dlambda2 = 2.*s2*rate_s2(lambda,s2);
}

int HotSpotRateCC::var(const char *name, double V,double e,const double *z,
                             double &value)
{
    if( !strcmp(name,"lambda") )
    {
        value = g(z[2]);
        return 0;
    }
    if( !strcmp(name,"ddt_lambda") )
    {
        value = dgds(z[2])*rate_s(V,e,z);
        return 0;
    }
    if( !strcmp(name,"lambda2") )
    {
        value = z[3]*z[3];
        return 0;
    }
    if( !strcmp(name,"ddt_lambda2") )
    {
        double lambda  = g(z[2]);
        double s2 = z[3];
        value = 2.*s2*rate_s2(lambda,s2);
        return 0;
    }
    if( !strcmp(name,"e_cc") )
    {
        double lambda  = g(z[2]);
        double lambda2 = z[3]*z[3];
        value = q(lambda,lambda2);
        return 0;
    }
    return 1;
}

int HotSpotRateCC::get_lambda(const double *z, double &lambda,double &lambda2)
{
    if( z == NULL )
        return 1;
    double s  = max(0.0,min(smax,z[2]));
    double s2 = max(0.0,min(1.0,z[3]));
    lambda  = g(s);
    lambda2 = s2*s2;
    return 0;
}

int HotSpotRateCC::set_lambda(double lambda, double lambda2, double *z)
{
    if( z == NULL )
        return 1;
    //lambda  = max(0.0, min(1.0,lambda));
    lambda2 = max(0.0, min(1.0,lambda2));
    z[2] = ginv(lambda);
    z[3] = sqrt(lambda2);
    return 0;
}
//
//
double HotSpotRateCC::g(double s)
{
    if( s <= 0.0 )
        return 0.0;
    else if( s < sc )
        return gf*(1.-exp(-s*s));
    else if( s < smax )
        return gf*(gc+(s-sc)*(dgc+0.5*(s-sc)*d2gc));
    else
        return 1.0;
}
double HotSpotRateCC::ginv(double lambda)
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
double HotSpotRateCC::dgds(double s)
{
    if( s <= 0.0 )
        return 0.0;
    else if( s < sc )
        return gf* 2.*s*exp(-s*s);
    else if( s < smax )
        return gf*(dgc+d2gc*(s-sc));
    else
        return 0.0;
}

double HotSpotRateCC::f(double Ps)
{
    double ans;
    if( fn > 0.0 )
    {
        if( Ps <= P0 )
            ans = 0.0;
        else if( Ps <= P1 )
            ans = C*pow((Ps-P0)/Pref,fn);
        else
            ans = f1*(1.0+df1*(1.0-exp(-B2*(Ps-P1)/Pref)));
    }
    else
    {
        if( df1 > 0.0 )
        {   // new form
            if( Ps <= P0 )
                ans = 0.0;
            else if( Ps <= P1 )
                ans = exp(A+B*Ps/Pref) - f0*(1.+B*(Ps-P0)/Pref);
            else
                ans = f1*(1.0+df1*(1.0 - exp(-B2*(Ps-P1)/Pref)));
        }
        else
        {   // old form: deprecated
            if( Ps < Pign )
                ans = exp(A+B*Ps/Pref);
            else if( Ps < Pmax )
                ans = ((Ps-Pign)*fmax+(Pmax-Ps)*fign) / (Pmax-Pign);
            else
                ans = fmax;
        }
    }
    return ans/tref;
}

double HotSpotRateCC::dfdp(double Ps)
{
    double ans;
    if( fn > 0.0 )
    {
            if( Ps <= P0 )
                ans = 0.0;
            else if( Ps <= P1 )
                ans = fn*C/Pref * pow((Ps-P0)/Pref,fn);
            else
                ans = f1*df1*(B2/Pref)* exp(-B2*(Ps-P1)/Pref);
    }
    else
    {
        if( df1 > 0.0 )
        {   // new form
            if( Ps <= P0 )
                ans = 0.0;
            else if( Ps <= P1 )
                ans = (B/Pref)*(exp(A+B*Ps/Pref) - f0);
            else
                ans = f1*df1*(B2/Pref)* exp(-B2*(Ps-P1)/Pref);
        }
        else
        {   // old form: deprecated
            if( Ps < Pign )
                ans = (B/Pref)*exp(A+B*Ps/Pref);
            else if( Ps < Pmax )
                ans = (fmax-fign) / (Pmax-Pign);
            else
                ans = 0.0;
        }
    }
    return ans/tref;
}

double HotSpotRateCC::h(double t)
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

double HotSpotRateCC::hinv(double lambda)
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

double HotSpotRateCC::rate_s(double V, double e, const double *z)
{
        double Ps = z[0];
        double rate = f(Ps);
        if( n != 0.0 )
        {
            double lambda  = g(z[2]);
            double lambda2 = z[3]*z[3];
            double p = P(V,e,lambda, lambda2);
            rate *= D(p/Ps);
        }
        return rate;
}

double HotSpotRateCC::rate_s2(double lambda, double s2)
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
//  HErate2 functions
//
double HotSpotRateCC::e2(double lambda2)
{
    lambda2 = max(0.0, min(1.0,lambda2));
    // Nratio = Neq/N0 > 1
    // eratio = 1/cbrt(Nratio)
    double ratio = lambda2*Nratio +(1.-lambda2);    // N(lambda2)/N(0)
    return (1./cbrt(ratio) - eratio)*Q;             // e2 >= 0
}
//
double HotSpotRateCC::q(double lambda, double lambda2)
{
    double ratio = lambda2*Nratio +(1.-lambda2);    // N(lambda2)/N(0)
    double de = (1./cbrt(ratio) - eratio)*Q;
    return lambda*de;                   // q = lambda*e2(lambda2) >= 0
}
double HotSpotRateCC::q(double lambda,double lambda2, double &dq1,double &dq2)
{
    // dq1 = dq/dlambda
    // dq2 = dq/dlambda2
    double ratio = lambda2*Nratio +(1.-lambda2);    // N(lambda2)/N(0)
    double cbrt_ratio = cbrt(ratio);
    double de = (1./cbrt_ratio - eratio)*Q;
    dq1 = de;                                               // dq1 >= 0
    dq2 = -(lambda*Q) * (Nratio-1.)/(3.*ratio*cbrt_ratio);  // dq2 <= 0
    return lambda*de;                   // q = lambda*e2(lambda2)  >= 0
}

int HotSpotRateCC::Rate(double V, double e, const double *z, double *zdot)
{
    double Ps = z[0];
    double ts = z[1];
    zdot[0] = zdot[1] = 0.0;
    zdot[2] = zdot[3] = 0.0;
    if( Ps <= 0.0 || ts < 0.0 )
        return 0;
    double s  = max(0.0,min(smax,z[2]));
    double s2 = max(0.0,min(1.0, z[3]));
    if( s == smax && s2 == 1. )
        return 0;   // equilibrium products (all burned)
    double lambda  = g(s);
    if( s < smax )
    {
        zdot[2] = f(Ps);
        if( n != 0 )
        {
            double p = P(V,e,lambda,s2*s2);
            if( isnan(p) )
                return 1;
            zdot[2] *= D(p/Ps);
        }
    }
    zdot[3] = rate_s2(lambda,s2);
    return 0;
}

int HotSpotRateCC::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0.0;
    if( source_type == 2 )
        return 2;               // stiff integrate, no time step constraint
    double Ps     = z[0];
    double ts     = z[1];
    if( Ps <= 0.0 || ts < 0.0 )
        return 0;               // no constraint, burn not yet started
    double s      = z[2];
    double s2     = z[3];
    if( s >= smax && s2 >= 1.  )
        return 0;               // no constraint, all burned
    double s_min = 0.02;        // make input parameter?
    s = max(s_min, min(0.98*smax,s));   // s_min < s < 0.98*smax
    double lambda  = g(s);
    double rate = dgds(s)*f(Ps);        // dlambda/dt
    if( n != 0 )
    {
        double p = P(V,e,lambda,s2*s2);
        if( isnan(p) )
            return -1;      // error
        rate *= D(p/Ps);
    }
    dt = dlambda/rate;
    double s2_max = 0.98;       // avoid rate=0 at s2=1, make input parameter?
    if( lambda>0.0 && s2 < s2_max )
    {
        rate = 2.*s2*rate_s2(lambda,s2);    // dlambda2/dt
        dt = min(dt, dlambda/rate);
    }
    return source_type;
}

int HotSpotRateCC::Integrate(double V, double e, double *z, double dt)
{
    if( HE == NULL )
        return 1;
    if( dt <= 0.0 )
        return 0;
    double Ps = z[0];
    double ts = z[1];
    if( Ps<=0.0 || ts<0.0 )
        return 0;       // burning not yet started
    double s  = max(0.0,z[2]);
    double s2 = max(0.0,z[3]);
    if( s >= smax && s2 >= 1. )
    {   // all burned
        z[2] = smax;    // s
        z[3] = 1.0;     // s2
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
            double lambda = g(s);
            double p = P(V,e, lambda,s2*s2);
            if( isnan(p) )
                return 1;
            Dp = D(p/Ps);
            // corrector
            double rate2 = rate_s2(lambda,s2);
            lambda = g(s+dt*Dp*rate0);
            double s2p = s2+dt*rate2;
            p = P(V,e, lambda,s2p*s2p);
            if( isnan(p) )
                return 1;
            Dp = 0.5*(Dp+D(p/Ps));
        }
        s += dt*Dp*rate0;
    }
    z[2] = min(s,smax);
    // second reaction
    double lambda = g(z[2]);
    double rate0 = rate_s2(lambda,s2);
    double rate1 = rate_s2(lambda,s2+dt*rate0);
    s2 += 0.5*dt*(rate0+rate1);
    z[3] = min(s2,1.);
    return 0;
}

double HotSpotRateCC::Dt(double V, double e, const double *z,
                         double &lambda, double &lambda2)
{
//  return dt such that lambda(t+dt)>=lambda and lambda2(t+dt)>=lambda2
    if( HE == NULL )
        return EOS::NaN;
    double Ps = z[0];
    double ts = z[1];
    if( Ps<=0.0 || ts<0.0 )
        return EOS::NaN;       // burning not yet started
    double s  = z[2];
    double s2 = z[3];
    double lambda_0  = g(s);
    double lambda2_0 = s2*s2;
    if( lambda_0>=lambda && lambda2_0>=lambda2 )
    {
        lambda  = lambda_0;
        lambda2 = lambda2_0;
        return 0;
    }
    double dt1 = 0.0;
    double s_t = ginv(lambda);
    double rate0 = f(Ps);
    if( lambda_0 < lambda )
    {
        double Dp = 1.0;
        if( n != 0 )
        {
            // predictor
            double p = P(V,e,lambda_0,lambda2_0);
            if( isnan(p) )
                return EOS::NaN;
            Dp = D(p/Ps);
            dt1 = (s_t - s)/(Dp*rate0);
            //second rate
            double rate2 = rate_s2(lambda_0,s2);
            s2 += rate2*dt1;
            // corrector
            p = P(V,e,lambda,s2*s2);
            if( isnan(p) )
                return EOS::NaN;
            Dp = 0.5*(Dp+D(p/Ps));
        }
        dt1 = (s_t - s)/(Dp*rate0);
        double rate1 = rate_s2(lambda,s2);
        double rate2 = rate_s2(lambda,s2+dt1*rate1);
        s2 = z[3] + 0.5*dt1*(rate1+rate2);
    }
    double dt2 = 0.0;
    if( s2*s2 < lambda2 )
    {
        double s2_t = sqrt(lambda2);
        double rate = 0.5*(rate_s2(lambda,s2)+rate_s2(lambda,s2_t));
        double dt2 = (s2_t-s2)/rate;
        if( n != 0 )
        {
            double p = P(V,e,lambda,lambda2);
            if( isnan(p) )
                return EOS::NaN;
            rate0 *= D(p/Ps);          
        }        
        s_t += dt2*rate0;
        lambda = g(s_t);
    }
    return dt1+dt2;
}
