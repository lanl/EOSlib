#include "IdealHE.h"
#include "IdealHE_Isotherm.h"

Isotherm_IdealHE::Isotherm_IdealHE(IdealHE &gas, double pvac)
                : Isotherm(gas,pvac)
{
    gamma_m = gas.gamma-1.;
    lambdaQ = gas.lambda_ref()*gas.Q;
    e0 = 0.; // set by Initialize
}

Isotherm_IdealHE::~Isotherm_IdealHE()
{
	// Null
}

int Isotherm_IdealHE::Initialize(const HydroState &state)
{
    e0 = state.e + lambdaQ;
    return Isotherm::Initialize(state);
}

int Isotherm_IdealHE::P(double p, ThermalState &state)
{
    int status = (p < p_vac) ? 1 : 0;
	p = max(p,p_vac);    
	state.e = state0.e;
	state.V = gamma_m*e0/p;	
	state.P = p;
	state.T = state0.T;	
	return status;
}

int Isotherm_IdealHE::V(double v, ThermalState &state)
{
	if( v <= 0 )
		return -1;		
	state.V = v;
	state.e = state0.e;
	state.P = gamma_m*e0/v;
    if( state.P < p_vac )
        return P(p_vac, state);
	state.T = state0.T;	
	return 0;
}
