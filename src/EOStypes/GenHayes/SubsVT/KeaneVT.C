#include <EOS_VT.h>
#include "Keane_VT.h"
#include <iostream>
using namespace std;


Keane_VT::Keane_VT() : GenHayes_VT("Keane")
{
    KeaneBase::eos = this;
}

Keane_VT::~Keane_VT()
{
    // Null
}


void Keane_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
    KeaneParams::PrintParams(out);
	out.setf(old, ios::adjustfield);	
}

int Keane_VT::ConvertParams(Convert &convert)
{
    int status = EOS_VT::ConvertParams(convert);
    if( status )
        return status;
    status = KeaneParams::ConvertParams(convert);
    if( status == 1 )
	{
	    EOSerror->Log( "Keane_VT::ConvertUnits", __FILE__, __LINE__,
                       this, "failed\n" );
	    return status;
	}
    if( InitODE(KeaneParams::eps_abs,KeaneParams::eps_rel) )
    {
        EOSerror->Log( "Keane_VT::ConvertUnits", __FILE__, __LINE__,
                       this, "Init ODE failed\n" );
        return 1;
    }    
	return 0;
}
