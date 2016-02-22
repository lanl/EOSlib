#include <iostream>

#include <EOS.h>
#include "IdealGas.h"

void IdealGas::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
	out << "\tgamma = " << gamma << "\n"
	    << "\t   Cv = " << Cv    << "\n";
}

