#include <iostream>

#include <EOS.h>
#include "StiffenedGas.h"

void StiffenedGas::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
	StiffGasParams::PrintParams(out);

	out << "\t#  P_ref = " << P(V_ref,e_ref)
	        << "; \tT_ref = " << T(V_ref,e_ref) << "\n"
	    << "\t#  c_ref = " << c(V_ref,e_ref)
            << "; \t    s = " <<  0.5*(1+0.5*G)  << "\n";
}


