#include <LocalMath.h>
#include <Arg.h>

#include "JWL_VT.h"
using namespace std;

int main(int, char **argv)
{
	ProgName(*argv);
	
	while(*++argv)
	{
		ArgError;
	}
	cout.setf(ios::showpoint);

    Parameters param;
    param.base = Strdup("EOS_VT");
    param.type = Strdup("JWL");
    param.name = Strdup("HMX");
    param.Append("V0=1/1.894");
    param.Append("e0=0");
    param.Append("A=858.0805");
    param.Append("B=7.546531");
    param.Append("C=0.781274");
    param.Append("R1=4.306");
    param.Append("R2=0.80");
    param.Append("omega=0.3");
    param.Append("Edet=11.0*V0");
    param.Append("Cv=1.e-3");
    param.Append("Tcj=3000");
 
	JWL_VT products;
    if( products.EOS_VT::Initialize(param) )
		cerr << Error("JWL.Initialize failed") << Exit;
    products.Print(cout);
    return 0;
}
