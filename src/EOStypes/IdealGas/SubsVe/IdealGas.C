#include <EOS.h>
#include "IdealGas.h"
#include <cmath>

IdealGas::IdealGas() : EOS("IdealGas"), gamma(5./3.), Cv(1.0)
{
	V_ref = 1.;
	e_ref = 1.;
	EOSstatus = EOSlib::good;
}

IdealGas::IdealGas(double g, double C_v) : EOS("IdealGas"),
					        gamma(g), Cv(C_v)
{
	V_ref = 1.;
	e_ref = 1.;
	EOSstatus = EOSlib::good;
}


IdealGas::~IdealGas()
{
	// Null
}

double IdealGas::P(double V, double e)
{
	return (gamma-1.)*e/V;
}

double IdealGas::T(double, double e)
{
	return e/Cv;
}

double IdealGas::S(double V, double e)
{
	return Cv*(log(e/e_ref) + (gamma-1.)*log(V/V_ref));
}

double IdealGas::c2(double, double e)
{
	return gamma*(gamma-1.)*e;
}

double IdealGas::Gamma(double, double)
{
	return gamma-1.;
}

double IdealGas::CV(double, double)
{
	return Cv;
}

double IdealGas::FD(double, double)
{
	return 0.5*(gamma+1.);
}

int IdealGas::PT(double P, double T, HydroState &state)
{
	if( P <= 0.0 || T <= 0.0 )
		return 1;
	
	state.e = Cv*T;
	state.V = (gamma-1.)*state.e/P;
	return 0;
}

double IdealGas::e_PV(double p, double V)
{
    return p*V/(gamma-1.);
}

int IdealGas::NotInDomain(double V, double e)
{
	return (V<=0. || e<0.) ? -1 : 0;
}
