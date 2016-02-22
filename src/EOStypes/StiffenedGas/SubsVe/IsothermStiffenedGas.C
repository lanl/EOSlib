#include <EOS.h>
#include "StiffenedGas.h"


int StiffenedGas::PT(double P, double T, HydroState &state)
{
	if( P+P_p <= 0 || T <= 0 )
		return 1;
	
	state.V = G*Cv*T/(P+P_p);
	state.e = Cv*T + e_p + P_p*state.V;
	return 0;
}


class Isotherm_StiffenedGas : public Isotherm
{
private:
	Isotherm_StiffenedGas();				// disallowed
	Isotherm_StiffenedGas(const Isotherm_StiffenedGas&);	// disallowed
	void operator=(const Isotherm_StiffenedGas&);		// disallowed
	Isotherm_StiffenedGas *operator &();			// disallowed
	
	StiffenedGas &gas;
public:
	Isotherm_StiffenedGas(StiffenedGas &Gas, double pvac=EOS::NaN); 
	~Isotherm_StiffenedGas();	

	int P(double p, ThermalState &state);
	int V(double v, ThermalState &state);
};

Isotherm_StiffenedGas::Isotherm_StiffenedGas(StiffenedGas &Gas, double pvac)
                        : Isotherm(Gas, pvac), gas(Gas)
{
    // Null
}

Isotherm_StiffenedGas::~Isotherm_StiffenedGas()
{
	// Nu;;
}

int Isotherm_StiffenedGas::P(double p, ThermalState &state)
{
    int status = (p < p_vac) ? 1 : 0;
	p = max(p,p_vac);
		
	double P_p = gas.P_p;
	double e_p = gas.e_p;
		
	state.V = state0.V*(state0.P+P_p)/(p+P_p);
	state.e = gas.Cv*state0.T+ e_p + P_p*state.V;
	
	state.P = p;
	state.T = state0.T;
	
	return status;
}

int Isotherm_StiffenedGas::V(double v, ThermalState &state)
{
	if( v <= 0 )
		return 1;
		
	double G = gas.G;
	double P_p = gas.P_p;
	double e_p = gas.e_p;

	state.V = v;
	state.e = gas.Cv*state0.T+ e_p + P_p*state.V;
	
	if( (state.P=G*(state.e-e_p)/v-(G+1)*P_p) < p_vac )
        return P(p_vac,state);  
	state.T = state0.T;	
	return 0;
}

Isotherm *StiffenedGas::isotherm(const HydroState &state)
{
	Isotherm *T = new Isotherm_StiffenedGas(*this);
	int status = T->Initialize(state);	
	if( status )
	{
	    EOSerror->Log("StiffenedGas::isotherm", __FILE__, __LINE__, this,
	                   "Initialize failed with status %d\n", status);
	    delete T;
	    T = NULL;
	}
	return T;
}
