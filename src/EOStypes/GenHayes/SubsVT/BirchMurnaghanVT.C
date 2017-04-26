#include <EOS_VT.h>
#include "BirchMurnaghan_VT.h"
#include <iostream>
using namespace std;


BirchMurnaghan_VT::BirchMurnaghan_VT() : GenHayes_VT("BirchMurnaghan")
{
    BirchMurnaghanBase::eos = this;
}

BirchMurnaghan_VT::~BirchMurnaghan_VT()
{
    // Null
}


void BirchMurnaghan_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
    BirchMurnaghanParams::PrintParams(out);
	out.setf(old, ios::adjustfield);	
}

int BirchMurnaghan_VT::ConvertParams(Convert &convert)
{
    int status = EOS_VT::ConvertParams(convert);
    if( status )
        return status;
    status = BirchMurnaghanParams::ConvertParams(convert);
    if( status == 1 )
	{
	    EOSerror->Log( "BirchMurnaghan_VT::ConvertUnits", __FILE__, __LINE__,
                       this, "failed\n" );
	    return status;
	}
    if( InitODE(BirchMurnaghanParams::eps_abs,BirchMurnaghanParams::eps_rel) )
    {
        EOSerror->Log( "BirchMurnaghan_VT::ConvertUnits", __FILE__, __LINE__,
                       this, "Init ODE failed\n" );
        return 1;
    }    
	return 0;
}
