#include <LocalMath.h>
#include <EOS.h>
#include <Isotherm_ODE.h>

#include "Uniaxial.h"


class UniaxialIsotherm : public Isotherm, private ODEfunc
{
	enum UniaxialIsotherm_type
	{
		pressure=0, specificV=1
	};
private:
	Isotherm_ODE solid;
	UniaxialEOS *vm_eos;
    EOSbase::Error *Error;
	
	UniaxialIsotherm_type type;
	double f(double V, const double *y, const double *yp); // ODEfunc::f

	UniaxialIsotherm();				            // disallowed
	UniaxialIsotherm(const UniaxialIsotherm&);	// disallowed
	void operator=(const UniaxialIsotherm&);	// disallowed
	UniaxialIsotherm *operator &();			    // disallowed
public:
	UniaxialIsotherm(UniaxialEOS &e, double pvac = EOS::NaN);
	int Initialize(const HydroState &state);
	~UniaxialIsotherm();

	int P(double p, ThermalState &state);
	int V(double v, ThermalState &state);
};

UniaxialIsotherm::UniaxialIsotherm(UniaxialEOS &e, double pvac)
    : Isotherm(e,pvac), vm_eos((UniaxialEOS*)e.Duplicate()),
		solid(*e.eos), Error(vm_eos->ErrorHandler())
{
    // Null
}

UniaxialIsotherm::~UniaxialIsotherm()
{
	EOS *e = static_cast<EOS*>(vm_eos);
	deleteEOS(e);
}

int UniaxialIsotherm::Initialize(const HydroState &state)
{
	if( Isotherm::Initialize(state) )
		return 1;

	return solid.Initialize(state);
}

double UniaxialIsotherm::f(double V, const double *y, const double *yp)
{
	switch (type)
	{
	    case pressure:
	    {	// e -> e + ShearEnergy() but stress_dev only depends on V
		double e = y[0];
		double stress = solid.Eos().P(V,e) + vm_eos->stress_dev(V,e);
		return -stress;
	    }
	    case specificV:
		return V;
	}
}

int UniaxialIsotherm::P(double p, ThermalState &state)
{
	double y[1];
	double yp[1];
	
	double V;
	type = pressure;
	double stress = -p;
	int status = solid.Integrate(*this, stress, V, y, yp);
	if( status )
	{
	    Error->Log("UniaxialIsotherm::P", __FILE__, __LINE__, vm_eos,
	               "Integrate failed with ODE status: %s\n",
		           solid.ErrorStatus(status) );
		return 1;
	}
	
	double e = y[0];
	
	state.V = V;
	// ShearEnergy depends only on V
	state.e = e + vm_eos->ShearEnergy(V, e);
	
	state.P = eos->P(state.V, state.e);
	state.T = eos->T(state.V, state.e);

	return abs(state.T-state0.T) > 1e-3*state0.T;
}

int UniaxialIsotherm::V(double v, ThermalState &state)
{
	double y[1];
	double yp[1];
	
	double V;
	type = specificV;
	int status = solid.Integrate(*this, v, V, y, yp);
	if( status )
	{
	    Error->Log("UniaxialIsotherm::V", __FILE__, __LINE__, vm_eos,
	               "Integrate failed with ODE status: %s\n",
		           solid.ErrorStatus(status) );
		return 1;
	}	
	double e = y[0];

	state.V = V;
	// ShearEnergy depends only on V
	state.e = e + vm_eos->ShearEnergy(V,e);
	
	state.P = eos->P(state.V, state.e);
	state.T = eos->T(state.V, state.e);

	return abs(state.T-state0.T) > 1e-3*state0.T;
}

Isotherm *UniaxialEOS::isotherm(const HydroState &state)
{
	Isotherm *T = new UniaxialIsotherm(*this);
	int status_T = T->Initialize(state);	
	if( status_T )
	{
	    EOSerror->Log("UniaxialEOS::isotherm", __FILE__, __LINE__, this,
	                   "Initialize failed with status: %d\n",
		               status_T );
		delete T;
		T = NULL;
	}
	
	return T;
}

