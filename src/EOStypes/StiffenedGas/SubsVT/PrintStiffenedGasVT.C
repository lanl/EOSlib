#include <iostream>

#include <EOS_VT.h>
#include "StiffenedGas_VT.h"

void StiffenedGas_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
	StiffGasParams::PrintParams(out);

    out << "\t#  P_ref = " << P(V_ref,T_ref)
	        << "; \te_ref = " << e(V_ref,T_ref) << "\n"
	    << "\t#  c_ref = " << c(V_ref,T_ref)
            << "; \t    s = " <<  0.5*(1+0.5*G) << "\n";
}
