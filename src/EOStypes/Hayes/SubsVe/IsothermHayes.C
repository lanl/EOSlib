#include <EOS.h>
#include "Hayes.h"

class Isotherm_Hayes : public Isotherm
{
private:
	Isotherm_Hayes();			// disallowed
	Isotherm_Hayes(const Isotherm_Hayes&);	// disallowed
	void operator=(const Isotherm_Hayes&);	// disallowed
	Isotherm_Hayes *operator &();		// disallowed
	
	Hayes &h;
public:
	Isotherm_Hayes(Hayes &e, double pvac = EOS::NaN); 
	~Isotherm_Hayes();	

	int P(double p, ThermalState &state);
	int V(double v, ThermalState &state);
};

Isotherm_Hayes::Isotherm_Hayes(Hayes &e, double pvac)
                    : Isotherm(e, pvac), h(e)
{
    // Null
}

Isotherm_Hayes::~Isotherm_Hayes()
{
	// Null
}

int Isotherm_Hayes::P(double p, ThermalState &state)
{
	if( p <= 0 )
		return 1;
		
	state.V = h.V_PT(p,state0.T);
	if( std::isnan(state.V) )
		return 1;
	state.e = h.e_VT(state.V,state0.T);
	
	state.P = p;
	state.T = state0.T;
	
	return 0;
}

int Isotherm_Hayes::V(double v, ThermalState &state)
{
	if( v <= 0 )
		return 1;
		
	state.V = v;
	state.e = h.e_VT(state.V,state0.T);
	
	state.P = h.P_VT(state.V,state0.T);
	state.T = state0.T;
	
	return 0;
}

Isotherm *Hayes::isotherm(const HydroState &state)
{
	Isotherm *T = new Isotherm_Hayes(*this);
	int status = T->Initialize(state);
	
	if( status )
	{
	    EOSerror->Log("Hayes::isotherm", __FILE__, __LINE__, this,
		           "initialization failed with status %d\n", status);
	    delete T;
	    T = NULL;
	}

	return T;
}

