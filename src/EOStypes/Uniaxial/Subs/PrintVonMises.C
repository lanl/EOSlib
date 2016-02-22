#include <iostream>
using namespace std;

#include <EOS.h>
#include "Uniaxial.h"

void VonMisesElasticPlastic::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\teos = ";
	if( eos )
	{
	    const char *type_e = eos->Type();
	    const char *name_e = eos->Name();
	    out << (type_e ? type_e : "NULL") << "::"
		    << (name_e ? name_e : "NULL") << "\n";
	}
	else
	    out << "NULL\n";
		
	out << "\t  G = " << G << "\n"
	    << "\t  Y = " << Y << "\n";
    out << "\t#  Vy_comp = " << Vy_comp << "\n";
    out << "\t#  Vy_exp  = " << Vy_exp  << "\n";
	out.setf(old, ios::adjustfield);	
}

ostream &VonMisesElasticPlastic::PrintComponents(ostream &out)
{
    if( eos )
        eos->PrintAll(out);
    return out;
}

