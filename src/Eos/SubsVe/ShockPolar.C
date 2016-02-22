#include <LocalMath.h>
#include "ShockPolar.h"

ShockPolar::ShockPolar(EOS &e)
{
    eos = e.Duplicate();
    hug = NULL;
    P_tol     = e.P_vac;
    theta_tol = 1e-8;
}

ShockPolar::~ShockPolar()
{
    delete hug;
    deleteEOS(eos);
}

void ShockPolar::Wave2Polar(WaveState &wave, int dir, PolarWaveState &polar)
{
    polar.V = wave.V;
    polar.e = wave.e;
    polar.P = wave.P;
    polar.q = sqrt(2.*(B - wave.e - wave.P*wave.V));
    double up = wave.u;
    double us = wave.us;
    double qpar = sqrt(state0.q*state0.q-us*us);
    double dtheta = atan(qpar*up/(state0.q*state0.q-us*up));
    polar.theta = state0.theta + dir*dtheta;
    polar.beta  = state0.theta + dir*asin(us/state0.q);
}

int ShockPolar::Initialize(PolarState &s)
{
    if( eos==NULL )
        return 1;
    state0 = s;
    HydroState hstate(state0.V,state0.e,0.0);
    P0 = eos->P(hstate);
    c0 = eos->c(hstate);
    B  = 0.5*state0.q*state0.q + state0.e + P0*state0.V;
    if( isnan(P0) || isnan(c0) || s.q < c0 )
        return 1;
    beta0_min = asin(c0/state0.q);
    hug = eos->shock(hstate);
    if( hug==NULL )
        return 1;
    if( hug->u_s(state0.q,RIGHT,Pmax) )
        return 1;
    // derived class must set dtheta_max and sonic
    return 0;
}

int ShockPolar::P(double p, int dir, PolarWaveState &polar)
{
    if( p < P0-P_tol || Pmax.P+P_tol < p)
        return 1;
    if( p <= P0 )
    {
        polar.V     = state0.V;
        polar.e     = state0.e;
        polar.q     = state0.q;
        polar.theta = state0.theta;
        polar.P     = P0;
        polar.beta  = state0.theta + dir*beta0_min;
        return 0;
    }
    if( Pmax.P <= p )
    {
        Wave2Polar(Pmax,dir,polar);        
        return 0;
    }
    if( hug==NULL || hug->P(p,RIGHT,pwave) )
        return 1;
    Wave2Polar(pwave,dir,polar);
    return 0;  
}

int ShockPolar::beta0(double b, int dir, PolarWaveState &polar)
{
    if( b < beta0_min-theta_tol || 0.5*PI+theta_tol < b )
        return 1;
    if( b <= beta0_min )
    {
        polar.V     = state0.V;
        polar.e     = state0.e;
        polar.q     = state0.q;
        polar.theta = state0.theta;
        polar.P     = P0;
        polar.beta  = state0.theta + dir*beta0_min;
        return 0;    
    }
    if( 0.5*PI <= b )
    {
        Wave2Polar(Pmax,dir,polar);        
        return 0;    
    }
    double us = state0.q*sin(b);
    if( hug==NULL || hug->u_s(us,RIGHT,pwave) )
        return 1;
    Wave2Polar(pwave,dir,polar);
    return 0;   
}
//
//
PrandtlMeyer::PrandtlMeyer(EOS &e)
{
    eos = e.Duplicate();    
}

PrandtlMeyer::~PrandtlMeyer()
{
    deleteEOS(eos);
}

int PrandtlMeyer::Initialize(PolarState &s)
{
    state0 = s;
    P0 = eos->P(s.V,s.e);
    c0 = eos->c(s.V,s.e);
    if( isnan(P0) || isnan(c0) || s.q < c0 )
        return 1;
    B = 0.5*s.q*s.q + s.e + P0*s.V;
    return 0;
}
