#include <EOS.h>
#include "IdealGas.h"
#include "IdealGas_Isentrope.h"

Isentrope *IdealGas::isentrope(const HydroState &state)
{
	Isentrope *S = new Isentrope_IdealGas(*this);
	int status = S->Initialize(state);
	
	if( status )
	{
	     EOSerror->Log("IdealGas::isentrope", __FILE__, __LINE__, this,
	                    "Initialize failed with status %d\n", status);
	     delete S;
	     S = NULL;
	}
	return S;
}

//

Isentrope_IdealGas::Isentrope_IdealGas(IdealGas &Gas, double pvac)
                        : Isentrope(Gas,pvac), gas(Gas)
{
    // Null
}


Isentrope_IdealGas::~Isentrope_IdealGas()
{
	// Null
}

int Isentrope_IdealGas::V(double V, int dir, WaveState &wave)
{
    if( V <= 0 )
        return -1;
	wave.V  = V;
	wave.e  = state0.e*pow(state0.V/V,gas.gamma-1);
	if( (wave.P=(gas.gamma-1)*wave.e/V) < p_vac )
        return P(p_vac,dir,wave);
	double c = sqrt(gas.gamma*wave.P*V);
	wave.u  = state0.u - dir * 2/(gas.gamma-1) *(state0.c - c);
	wave.us = wave.u + dir * c;	// trailing edge
	return 0;
}

int Isentrope_IdealGas::P(double p, int dir, WaveState &wave)
{
    int status = (p<p_vac) ? 1 : 0;
    p = max(p,p_vac);
	wave.V = state0.V * pow(state0.P/p, 1/gas.gamma);
	wave.e = p*wave.V/(gas.gamma-1);
	wave.P = p;
	double c = sqrt(gas.gamma*wave.P*wave.V);
	wave.u  = state0.u - dir * 2/(gas.gamma-1) *(state0.c - c);
	wave.us = wave.u + dir * c;	// trailing edge
	return status;	
}

int Isentrope_IdealGas::u(double u, int dir, WaveState &wave)
{
	double c = state0.c + dir *0.5*(gas.gamma-1)*(u -state0.u);
	if( c < 0 )
        return P(p_vac,dir,wave);
	wave.V = state0.V * pow(state0.c/c,2/(gas.gamma-1));
	if( (wave.P=c*c/(gas.gamma*wave.V)) < p_vac )
        return P(p_vac,dir,wave);
	wave.e = wave.P*wave.V/(gas.gamma-1); 	
	wave.u = u;
	wave.us = wave.u + dir * c;	// trailing edge
	return 0;	
}

int Isentrope_IdealGas::u_s(double us, int dir, WaveState &wave)
{
	double g = gas.gamma;
	double c = (2*state0.c + dir*(g-1)*(us -state0.u))/(g+1);
	if( c < 0 )
        return P(p_vac,dir,wave);
	wave.V = state0.V * pow(state0.c/c,2/(g-1));
	if( (wave.P=c*c/(g*wave.V)) < p_vac )
        return P(p_vac,dir,wave);
	wave.e = wave.P*wave.V/(g-1); 	
	wave.us = us;			// trailing edge
	wave.u  = us -dir*c;
	return 0;	
}

double Isentrope_IdealGas::u_escape(int dir)
{
	return state0.u - dir*2/(gas.gamma-1)*state0.c*
                    (1.-pow(p_vac/state0.P, 0.5*(gas.gamma-1)/gas.gamma));
}

int Isentrope_IdealGas::u_escape(int direction, WaveState &wave)
{
     return P(p_vac, direction, wave);   
}

