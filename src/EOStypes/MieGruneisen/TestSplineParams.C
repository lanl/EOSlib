#include <Arg.h>
#include <LocalMath.h>
#include "UsUp.h"
#include "UsUpSplineParams.h"

// Access to protected members of UpUsSplineParams
class SplineParams : public UsUpSplineParams
{
public:
    SplineParams() {}
    UsUpSplineParams::init;    
};

//

int main(int, char **argv)
{
	ProgName(*argv);
    double s1 = 1.5;
    double dusdup0 = EOS::NaN;
    int n_data = 10;

    double c0 =1;
    double c1 = 1.5;
    double a=5;
    int nsteps = 20;
    double eta_max = 0.2;
    int print = 0;
	while(*++argv)
	{
        GetVar(s1,s1);
        GetVar(dusdup0,dusdup0);
        GetVar(n_data,n_data);
        GetVarValue(print,print,1);
        
        GetVar(c0,c0);
        GetVar(c1,c1);
        GetVar(a,a);       
        GetVar(nsteps,nsteps);       
        GetVar(eta_max,eta_max);       
		ArgError;
	}

    // Initialize
    SplineParams f;
    f.V0 =1/1.9;
    f.P0 = 1e-4;
    f.e0 = f.S0 = 0.0;
    f.T0 = 300;
    f.G0 = f.G1 = 1.1;
    f.Cv = 1e-3;

    f.s1  = s1;
    f.dusdup0 = dusdup0;
    f.n_data = n_data;
    f.up_data = new double[n_data];
    f.us_data = new double[n_data];

    double eta1 = 1/s1;
    int i;
    for( i=0; i<n_data; i++ )
    {
        double eta = i*(eta1/n_data);
        double us_inv = (1-s1*eta)*(1 +(c1/c0-1)*exp(-a*s1*eta))/c1;
        f.us_data[i] = 1/us_inv;
        f.up_data[i] = eta/us_inv;
    }
	UsUp solid;
    if( f.init(solid.ErrorHandler()) )
		cerr << Error("f.init failed") << Exit;
    if( print )
    {
        f.PrintParams(cout);
        return 0;
    }
    // Test
    if(eta_max <= 0 || eta_max > eta1)
        eta_max = eta1;
    for( i=0; i<nsteps; i++ )
    {
        double eta = i*(eta_max/nsteps);
        double us = f.Us(eta);
        double up = eta*us;
        cout << up << " " << us << "\n";       
    }

    return 0;
}
