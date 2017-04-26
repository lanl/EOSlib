#include <EOS.h>
#include "HayesBM.h"

/*****************************************************************************
// Needs  HayesBM::V_PT(double P, double T)
// 
class Isotherm_HayesBM : public Isotherm
{
private:
	Isotherm_HayesBM();			// disallowed
	Isotherm_HayesBM(const Isotherm_HayesBM&);	// disallowed
	void operator=(const Isotherm_HayesBM&);	// disallowed
	Isotherm_HayesBM *operator &();		// disallowed
	
	HayesBM &h;
public:
	Isotherm_HayesBM(HayesBM &e, double pvac = EOS::NaN); 
	~Isotherm_HayesBM();	

	int P(double p, ThermalState &state);
	int V(double v, ThermalState &state);
};

Isotherm_HayesBM::Isotherm_HayesBM(HayesBM &e, double pvac)
                    : Isotherm(e, pvac), h(e)
{
    // Null
}

Isotherm_HayesBM::~Isotherm_HayesBM()
{
	// Null
}

int Isotherm_HayesBM::P(double p, ThermalState &state)
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

int Isotherm_HayesBM::V(double v, ThermalState &state)
{
	if( v <= 0 )
		return 1;
		
	state.V = v;
	state.e = h.e_VT(state.V,state0.T);
	
	state.P = h.P_VT(state.V,state0.T);
	state.T = state0.T;
	
	return 0;
}

Isotherm *HayesBM::isotherm(const HydroState &state)
{
	Isotherm *T = new Isotherm_HayesBM(*this);
	int status = T->Initialize(state);
	
	if( status )
	{
	    EOSerror->Log("HayesBM::isotherm", __FILE__, __LINE__, this,
		           "initialization failed with status %d\n", status);
	    delete T;
	    T = NULL;
	}

	return T;
}
*****************************************************************************/
