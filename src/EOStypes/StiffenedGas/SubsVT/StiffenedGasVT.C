#include <EOS_VT.h>
#include "StiffenedGas_VT.h"

StiffenedGas_VT::StiffenedGas_VT() : EOS_VT("StiffenedGas")
{
// degenerate to ideal gas
    G = 2./3.;
	P_p = 0.0;
	e_p = 0.0;
    Cv = 1.0;
	V_ref = 1;
	T_ref = 300;

	EOSstatus = EOSlib::good;
}

StiffenedGas_VT::StiffenedGas_VT(double g, double e, double p, double C_v)
	         : EOS_VT("StiffenedGas")
{
    G     = g;
    e_p   = e;
    P_p   = p;
    Cv    = C_v;
	V_ref = 1;
	T_ref = 300;

	EOSstatus = EOSlib::good;
}


StiffenedGas_VT::~StiffenedGas_VT()
{
	// null
}
