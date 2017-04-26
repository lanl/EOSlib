#include <iostream>
using namespace std;
#include <EOS.h>
#include "Keane.h"

Keane::Keane() : GenHayes("Keane")
{
    KeaneBase::eos = this;
}

Keane::~Keane()
{
    // Null
}

void Keane::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
    KeaneParams::PrintParams(out);
	out.setf(old, ios::adjustfield);	
}

int Keane::ConvertParams(Convert &convert)
{
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
    status = KeaneParams::ConvertParams(convert);
    if( status == 1 )
	{
	    EOSerror->Log("Keane::ConvertUnits", __FILE__, __LINE__, this,
	                   "failed\n");
	    return status;
	}
    if( InitODE(KeaneParams::eps_abs,KeaneParams::eps_rel) )
    {
        EOSerror->Log("Keane::ConvertUnits", __FILE__, __LINE__, this,
                       "Init ODE failed\n");
        return 1;
    }    
	return 0;
}
