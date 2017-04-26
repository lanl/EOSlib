#include "IdealHE.h"
#include "IdealHE_Isentrope.h"

Isentrope_IdealHE::Isentrope_IdealHE(IdealHE &gas, double pvac)
                        : Isentrope(gas,pvac)
{
    gamma   = gas.gamma;
    lambdaQ = gas.lambda_ref()*gas.Q;
    e0 = 0.0;   // set by Initialize()
}


Isentrope_IdealHE::~Isentrope_IdealHE()
{
	// Null
}

int Isentrope_IdealHE::Initialize(const HydroState &state)
{
    e0 = state.e + lambdaQ;
    return Isentrope::Initialize(state);
}

int Isentrope_IdealHE::V(double V, int dir, WaveState &wave)
{
    if( V <= 0 )
        return -1;
	wave.V  = V;
    wave.P = state0.P*pow(state0.V/V,gamma);
	if( wave.P < p_vac )
        return P(p_vac,dir,wave);
    wave.e = wave.P*V/(gamma-1.) -lambdaQ;
	double c = sqrt(gamma*wave.P*V);
	wave.u  = state0.u - dir*2./(gamma-1.)*(state0.c-c);
	wave.us = wave.u + dir*c;	// trailing edge
	return 0;
}

int Isentrope_IdealHE::P(double p, int dir, WaveState &wave)
{
    int status = (p<p_vac) ? 1 : 0;
    p = max(p,p_vac);
	wave.V = state0.V * pow(state0.P/p, 1./gamma);
	wave.e = p*wave.V/(gamma-1.)-lambdaQ;
	wave.P = p;
	double c = sqrt(gamma*wave.P*wave.V);
	wave.u  = state0.u - dir*2./(gamma-1.)*(state0.c-c);
	wave.us = wave.u + dir*c;	// trailing edge
	return status;	
}

int Isentrope_IdealHE::u(double u, int dir, WaveState &wave)
{
	double c = state0.c + dir*0.5*(gamma-1.)*(u-state0.u);
	if( c < 0 )
        return P(p_vac,dir,wave);
	wave.V = state0.V * pow(state0.c/c,2./(gamma-1.));
	if( (wave.P=c*c/(gamma*wave.V)) < p_vac )
        return P(p_vac,dir,wave);
	wave.e = wave.P*wave.V/(gamma-1.)-lambdaQ; 	
	wave.u = u;
	wave.us = wave.u + dir*c;	// trailing edge
	return 0;	
}

int Isentrope_IdealHE::u_s(double us, int dir, WaveState &wave)
{
	double c = (2.*state0.c+dir*(gamma-1.)*(us-state0.u))/(gamma+1.);
	if( c < 0 )
        return P(p_vac,dir,wave);
	wave.V = state0.V * pow(state0.c/c,2./(gamma-1.));
	if( (wave.P=c*c/(gamma*wave.V)) < p_vac )
        return P(p_vac,dir,wave);
	wave.e = wave.P*wave.V/(gamma-1.)-lambdaQ; 	
	wave.us = us;			// trailing edge
	wave.u  = us-dir*c;
	return 0;	
}

double Isentrope_IdealHE::u_escape(int dir)
{
	return state0.u - dir*2./(gamma-1.)*state0.c*
                    (1.-pow(p_vac/state0.P, 0.5*(gamma-1.)/gamma));
}

int Isentrope_IdealHE::u_escape(int direction, WaveState &wave)
{
     return P(p_vac, direction, wave);   
}
