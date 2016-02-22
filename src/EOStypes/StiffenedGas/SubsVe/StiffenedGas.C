#include <EOS.h>
#include "StiffenedGas.h"
#include <cmath>

StiffenedGas::StiffenedGas() : EOS("StiffenedGas")
{
// degenerate to ideal gas
    G = 2./3.;
	P_p = 0.0;
	e_p = 0.0;
    Cv  = 1.0;
	V_ref = 1.;
	e_ref = 1.;

	EOSstatus = EOSlib::good;
}

StiffenedGas::StiffenedGas(double g, double e, double p, double C_v)
	         : EOS("StiffenedGas")
{
    G     = g;
    e_p   = e;
    P_p   = p;
    Cv    = C_v;
	V_ref = 1.;
	e_ref = 1.;

	EOSstatus = EOSlib::good;
}


StiffenedGas::~StiffenedGas()
{
	// null
}

double StiffenedGas::P(double V, double e)
{
	return G*(e-e_p)/V - (G+1.)*P_p;
}

double StiffenedGas::T(double V, double e)
{
	return (e-e_p-P_p*V)/Cv;
}

double StiffenedGas::S(double V, double e)
{
	return Cv*(log((e-e_p-P_p*V)/(e_ref-e_p-P_p*V_ref)) + G*log(V/V_ref));
}

double StiffenedGas::c2(double V, double e)
{
	return (G+1.)*G*(e-e_p-P_p*V);
}

double StiffenedGas::Gamma(double, double)
{
	return G;
}

double StiffenedGas::CV(double, double)
{
	return Cv;
}

double StiffenedGas::FD(double, double)
{
	return 1. + 0.5*G;
}

double StiffenedGas::e_PV(double p, double V)
{
	return e_p + V/G *(p + (G+1.)*P_p);
}

int StiffenedGas::NotInDomain(double V, double e)
{
	return (V<=0. || P(V,e)<0.) ? -1 : 0;
}
