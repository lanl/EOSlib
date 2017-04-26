#include <EOSbase.h>
#include "StiffGasParams.h"

StiffGasParams::StiffGasParams()
    : G(EOSbase::NaN), e_p(EOSbase::NaN), P_p(EOSbase::NaN), Cv(EOSbase::NaN)
{
    // Null
}

int StiffGasParams::ConvertParams(Convert &convert)
{
	double s_V, s_e, s_C;
	if( !finite(s_C = convert.factor("Cv"))
	     || !finite(s_V = convert.factor("V"))
	         || !finite(s_e = convert.factor("e"))	)
	{
	    return 1;
	}
	Cv  *= s_C;
	e_p *= s_e;
	P_p *= s_e/s_V;
	
	return 0;
}

#define calcvar(var) calc.Variable(#var,&var)
void StiffGasParams::InitParams(Calc &calc)
{
    G = e_p = P_p = Cv = EOSbase::NaN;
	calcvar(G);
	calcvar(e_p);
	calcvar(P_p);
	calcvar(Cv);
}

void StiffGasParams::PrintParams(ostream &out) const
{
	out << "\tGamma = " << G   << "\n"
	    << "\t  e_p = " << e_p << "; \tP_p = " << P_p << "\n"
	    << "\t   Cv = " << Cv  << "\n";
}

int StiffGasParams::ParamsOK() const
{
    return finite(G) && finite(e_p) && finite(P_p) && finite(Cv);
}
