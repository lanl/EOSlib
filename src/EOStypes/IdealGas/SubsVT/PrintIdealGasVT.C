#include <iostream>

#include <EOS_VT.h>
#include "IdealGas_VT.h"

void IdealGas_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
	out << "\tgamma = " << gamma << "\n"
	    << "\t   Cv = " << Cv    << "\n";
}

