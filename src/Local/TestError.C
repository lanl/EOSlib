#include <iostream>
#include "ErrorHandler.h"

int main(int, char **)
{
	cerr << "***  Testing Log\n";
	LocalError->Log("main","x = %f",1.);


	cerr << "\n***  Testing Throw\n";
	try
	{
	    LocalError->Throw("main",__FILE__, __LINE__,"throw exception");
	    // cerr << "skip throw\n";
	}
	catch(LocalErrorHandler e)
	{
	    *e.out << "Caught LocalErrorHandler exception\n";	
	}

	cerr << "\n***  Testing ABORT\n";
	LocalError->Abort("main",__FILE__, __LINE__,"x = %f\n",2.);

	cerr << "Error if this statement reached\n";
	return 0;
}
