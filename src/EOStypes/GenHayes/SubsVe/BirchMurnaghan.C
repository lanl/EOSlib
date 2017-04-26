#include <iostream>
using namespace std;

#include <EOS.h>
#include "BirchMurnaghan.h"

BirchMurnaghan::BirchMurnaghan() : GenHayes("BirchMurnaghan")
{
    BirchMurnaghanBase::eos = this;
}

BirchMurnaghan::~BirchMurnaghan()
{
    // Null
}

void BirchMurnaghan::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
    BirchMurnaghanParams::PrintParams(out);
	out.setf(old, ios::adjustfield);	
}

int BirchMurnaghan::ConvertParams(Convert &convert)
{
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
    status = BirchMurnaghanParams::ConvertParams(convert);
    if( status == 1 )
	{
	    EOSerror->Log("BirchMurnaghan::ConvertUnits", __FILE__, __LINE__, this,
	                   "failed\n");
	    return status;
	}
    if( InitODE(BirchMurnaghanParams::eps_abs,BirchMurnaghanParams::eps_rel) )
    {
        EOSerror->Log("BirchMurnaghan::ConvertUnits", __FILE__, __LINE__, this,
                       "Init ODE failed\n");
        return 1;
    }    
	return 0;
}
