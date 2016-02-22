#include "Porous.h"
#include <Isotherm_ODE.h>

class DissipativeIsotherm : public Isotherm, private ODEfunc
{
	enum DissipativeIsotherm_type
	{
		pressure=0, specificV=1
	};
private:
	Isotherm_ODE solid;
	PhiEq *phi_eq;
	
	DissipativeIsotherm_type type;
	double f(double V, const double *y, const double *yp); // ODEfunc::f

	DissipativeIsotherm();					// disallowed
	DissipativeIsotherm(const DissipativeIsotherm&);	// disallowed
	void operator=(const DissipativeIsotherm&);		// disallowed
	DissipativeIsotherm *operator &();			// disallowed
public:
	DissipativeIsotherm(DissipativePorous &e, double pvac = EOS::NaN);
	~DissipativeIsotherm();
	int Initialize(const HydroState &state);

	int P(double p, ThermalState &state);
	int V(double v, ThermalState &state);
};

DissipativeIsotherm::DissipativeIsotherm(DissipativePorous &e, double pvac)
        : Isotherm(e,pvac), solid(*e.solid), phi_eq(e.phi_eq->Duplicate())
{
    // Null
}

DissipativeIsotherm::~DissipativeIsotherm()
{
    deletePhiEq(phi_eq);
}


int DissipativeIsotherm::Initialize(const HydroState &state)
{
	if( Isotherm::Initialize(state) )
		return 1;

	double p = eos->P(state.V,state.e);
	double phi = phi_eq->phi(p*state.V);
	HydroState state_s(phi*state.V, state.e);
	return solid.Initialize(state_s);
}

double DissipativeIsotherm::f(double V, const double *y, const double *yp)
{
	double e = y[0];
	double p = solid.Eos().P(V,e);
	double phi = phi_eq->phi(p*V);

	switch (type)
	{
	    case pressure:
		return -phi*p;
	    case specificV:
		return V/phi;
	}
}

int DissipativeIsotherm::P(double p, ThermalState &state)
{
	double y[1];
	double yp[1];
	
	double val = -max(p,0.0);
	double V;
	type = pressure;
	int status = solid.Integrate(*this, val, V, y, yp);
	if( status )
	{
	    eos->ErrorHandler()->Log("DissipativeIsotherm::P",
		     __FILE__, __LINE__, eos,
	             "failed with ODE status: %s\n",
	              solid.ErrorStatus(status) );
	    return 1;
	}
	
	double e = y[0];
	p = solid.Eos().P(V,e);
	double phi = phi_eq->phi(p*V);
	
	state.V = V/phi;
	state.e = y[0];
	
	state.P = phi*p;
	state.T = eos->T(state.V, state.e);

	return abs(state.T-state0.T) > 1e-3*state0.T;
}

int DissipativeIsotherm::V(double v, ThermalState &state)
{
	double y[1];
	double yp[1];
	
	double V;
	type = specificV;
	int status = solid.Integrate(*this, v, V, y, yp);
	if( status )
	{
	    eos->ErrorHandler()->Log("DissipativeIsotherm::V",
		     __FILE__, __LINE__, eos,
	             "failed with ODE status: %s\n",
	             solid.ErrorStatus(status) );
	    return 1;
	}
	
	double e = y[0];
	double p = solid.Eos().P(V,e);
	double phi = phi_eq->phi(p*V);
	
	state.V = V/phi;
	state.e = y[0];
	
	state.P = phi*p;
	state.T = eos->T(state.V, state.e);

	return abs(state.T-state0.T) > 1e-3*state0.T;
}

Isotherm *DissipativePorous::isotherm(const HydroState &state)
{
	Isotherm *T = new DissipativeIsotherm(*this);
	int status_T = T->Initialize(state);
	
	if( status_T )
	{
	    EOSerror->Log("DissipativePorous::isotherm",
		   __FILE__, __LINE__, this, "Initialize failed\n" );
	    delete T;
	    return NULL;
	}
	
	return T;
}
