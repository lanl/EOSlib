#include <EOS.h>
#include "IdealGas.h"
#include "IdealGas_Isotherm.h"

Isotherm *IdealGas::isotherm(const HydroState &state)
{
	Isotherm *T = new Isotherm_IdealGas(*this);
	int status = T->Initialize(state);
	
	if( status )
	{
	    EOSerror->Log("IdealGas::isotherm", __FILE__, __LINE__, this,
	                   "Initialize failed with status %d\n", status);
	    delete T;
	    T = NULL;
	}
	return T;
}

//
    
Isotherm_IdealGas::Isotherm_IdealGas(IdealGas &Gas, double pvac)
                : Isotherm(Gas,pvac), gas(Gas)
{
    // Null
}

Isotherm_IdealGas::~Isotherm_IdealGas()
{
	// Null
}

int Isotherm_IdealGas::P(double p, ThermalState &state)
{
    int status = (p < p_vac) ? 1 : 0;
	p = max(p,p_vac);
    
	state.e = state0.e;
	state.V = (gas.gamma-1)*state.e/p;
	
	state.P = p;
	state.T = state0.T;
	
	return status;
}

int Isotherm_IdealGas::V(double v, ThermalState &state)
{
	if( v <= 0 )
		return -1;
		
	state.V = v;
	state.e = state0.e;
	
	if( (state.P=(gas.gamma-1)*state.e/v) < p_vac )
        return P(p_vac, state);
	state.T = state0.T;
	
	return 0;
}


