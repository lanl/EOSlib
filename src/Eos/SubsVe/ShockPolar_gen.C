#include <LocalMath.h>
#include "ShockPolar_gen.h"

ShockPolar_gen::ShockPolar_gen(EOS &e) : ShockPolar(e)
{
    // Null
}

ShockPolar_gen::~ShockPolar_gen()
{
    // Null
}

double ShockPolar_gen::f(double p)
{
    double value;
    switch (type)
    {
    case THETA:
        if( hug==NULL || hug->P(p,RIGHT,pwave) )
        {
            SetError();
            value = 0.;
        }
        else
        {
            double up = pwave.u;
            double us = pwave.us;
            double qpar = sqrt(state0.q*state0.q-us*us);
            value = atan(qpar*up/(state0.q*state0.q-us*up));
        }
        break;
    case THETA_MAX:
        // Henderson and Menikoff, Triple shock entropy theorem
        // Eq. (B2) q0^2 = us^2 + (us-up)*up*d(us)/d(up)
        if( hug==NULL || hug->P(p,RIGHT,pwave) )
        {
            SetError();
            value = 0.;
        }
        else
        {
            double up  = pwave.u;
            double us  = pwave.us;
            double eta = pwave.V/state0.V;
            double c2  = eos->c2(pwave.V,pwave.e);
            double Gamma = eos->Gamma(pwave.V,pwave.e);
            double eu2 = sqr(eta*us);
            double q2  = sqr(state0.q);
            value = q2-us*us*(1.+eta*(c2-eu2)/(c2-Gamma*eta*up*us+eu2));
        }        
        break;
    case SONIC:
        if( hug==NULL || hug->P(p,RIGHT,pwave) )
        {
            SetError();
            value = 0.;
        }
        else
        {
            double q2 = 2.*(B - pwave.e - pwave.P*pwave.V);
            double c2 = eos->c2(pwave.V,pwave.e);
            return c2-q2;
        }        
        break;
    }
    return value;
}

int ShockPolar_gen::Initialize(PolarState &s)
{
    if( ShockPolar::Initialize(s) )
        return 1;
    int dir = 1;
    double theta0 = state0.theta;
    state0.theta = 0.;
    // dtheta_max
    type = THETA_MAX;
    double p = zero(P0,Pmax.P);
    if( Status() )
    {
        eos->ErrorHandler()->Log("ShockPolar_gen::Initialize", __FILE__, __LINE__,
             eos, "inverse failed for THETA_MAX with status: %s\n",
             ErrorStatus());
        return 1;
    }
    Wave2Polar(pwave,dir,dtheta_max);   
    // sonic
    type = SONIC;
    p = zero(P0,Pmax.P);
    if( Status() )
    {
        eos->ErrorHandler()->Log("ShockPolar_gen::Initialize", __FILE__, __LINE__,
             eos, "inverse failed for SONIC with status: %s\n",
             ErrorStatus());
        return 1;
    }
    Wave2Polar(pwave,dir,sonic);
    state0.theta = theta0;
    return 0;
}
    
int ShockPolar_gen::ThetaLow(double theta, int dir, PolarWaveState &polar)
{
    double dtheta = dir*(theta -state0.theta);
    if( dtheta < -theta_tol || dtheta_max.theta+theta_tol < dtheta )
        return 1;
    if( dtheta <= 0.0 )
    {
        polar.V     = state0.V;
        polar.e     = state0.e;
        polar.q     = state0.q;
        polar.theta = state0.theta;
        polar.P     = P0;
        polar.beta  = state0.theta + dir*beta0_min;
        return 0;    
    }
    if( dtheta_max.theta < dtheta )
    {
        polar.V     = dtheta_max.V;
        polar.e     = dtheta_max.e;
        polar.q     = dtheta_max.q;
        polar.P     = dtheta_max.P;
        polar.theta = state0.theta + dir*dtheta_max.theta;
        polar.beta  = state0.theta + dir*dtheta_max.beta;
        return 0;
    }
    double x0 = P0;
    double y0 = 0.0;
    double x1 = dtheta_max.P;
    double y1 = dtheta_max.theta;
    type = THETA;
    double p = inverse(dtheta,x0,y0,x1,y1);
    if( Status() )
    {
        eos->ErrorHandler()->Log("ShockPolar_gen::ThetaLow", __FILE__, __LINE__,
             eos, "inverse failed with status: %s\n", ErrorStatus());
        return 1;
    }
    Wave2Polar(pwave,dir,polar);
    return 0;    
}

int ShockPolar_gen::ThetaHi(double theta, int dir, PolarWaveState &polar)
{
    double dtheta = dir*(theta -state0.theta);
    if( dtheta < -theta_tol || dtheta_max.theta+theta_tol < dtheta )
        return 1;
    if( dtheta <= 0.0 )
    {
        Wave2Polar(Pmax,dir,polar);        
        return 0;    
    }
    if( dtheta_max.theta < dtheta )
    {
        polar.V     = dtheta_max.V;
        polar.e     = dtheta_max.e;
        polar.q     = dtheta_max.q;
        polar.P     = dtheta_max.P;
        polar.theta = state0.theta + dir*dtheta_max.theta;
        polar.beta  = state0.theta + dir*dtheta_max.beta;
        return 0;
    }
    double x0 = Pmax.P;
    double y0 = 0.0;
    double x1 = dtheta_max.P;
    double y1 = dtheta_max.theta;
    type = THETA;
    double p = inverse(dtheta,x0,y0,x1,y1);
    if( Status() )
    {
        eos->ErrorHandler()->Log("ShockPolar_gen::ThetaHi", __FILE__, __LINE__,
             eos, "inverse failed with status: %s\n", ErrorStatus());
        return 1;
    }
    Wave2Polar(pwave,dir,polar);
    return 0;    
}
