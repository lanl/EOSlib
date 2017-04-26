#include <Arg.h>
#include <LocalMath.h>
#include "UsUp.h"
#include "UsUpSplineParams.h"
// Access to protected members of UpUsSplineParams
class SplineParams : public UsUpSplineParams
{
public:
    SplineParams() {}
    using UsUpSplineParams::init;    
};

// The dynamic mechanical behavior of polymethyl methacrylate
// K. W. Schuler and J. W. Nunziato
// Rheol. Acta. 13 (1974) pp. 265-273.

double rho0 = 1.185;
double V0 = 1./rho0;
double G0 = 0.5;
double GoV = G0/V0;
int np=3;
double pe[] = {8.979, 70.0, -586.9, 1965.2};    // GPa, footnote 5
// P(eta) = eta*sum pe[n]*eta^n          # Eq. [3.8]
// where eta = 1 - V/V0 is strain
double dusdup0 = 0.5*pe[1]/pe[0];
double Pe(double eta)
{
    double sum = 0.0;
    int i;
    for( i=np; i > 0 ; i-- )
    {
        sum += pe[i];
        sum *= eta;
    }
    return pe[0] + sum;   
}
// stress
inline double P(double eta){ return eta*Pe(eta);}
// shock velocity
inline double Us(double eta) { return sqrt(V0*Pe(eta));}
// particle velocity
inline double Up(double eta) { return eta*sqrt(V0*Pe(eta));}
// Lagrangian sound speed cl = rho*c
double CL(double eta)
{
    double sum = 0.0;
    int i;
    for( i=np; i > 0 ; i-- )
    {
        sum += (i+1)*pe[i];
        sum *= eta;
    }
    return sqrt(V0*(pe[0] + sum));
}
// sound speed
double C(double eta)
{
    double cl2 = sqr(CL(eta));
    double us2 = sqr(Us(eta));
    double c2 = cl2-0.5*(cl2-us2)*eta*V0*GoV;
    return (1-eta)*sqrt(c2);       
}
//
//
int main(int, char **argv)
{
	ProgName(*argv);
    double s1 = 1.54;   // Marsh data
    double eta_data = 0.23;
    int n_data = 10;
    //double G0 = G0;

    double eta_max = 0.23;
    int nsteps = 20;
    int print = 0;
    while(*++argv)
	{
        GetVar(s1,s1);
        GetVar(dusdup0,dusdup0);
        GetVar(n_data,n_data);
        GetVar(eta_data,eta_data);
        GetVarValue(print,print,1);
        GetVarValue(points,print,2);
        
        GetVar(eta_max,eta_max);
        GetVar(nsteps,nsteps);
	    ArgError;
	}
	cout.setf(ios::showpoint);
    // Initialize
    SplineParams f;
    f.V0 = V0;
    f.P0 = 0.0;
    f.e0 = f.S0 = 0.0;
    f.T0 = 300;
    f.G0 = f.G1 = G0;
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
        double eta = i*(eta_data/n_data);
        f.us_data[i] = Us(eta);
        f.up_data[i] = eta*f.us_data[i];
        if( print == 2 )
            cout << f.up_data[i] << " " << f.us_data[i] << "\n";
    }
    if( print == 2 )
        return 0;
   	UsUp solid;
    if( f.init(solid.ErrorHandler()) )
		cerr << Error("f.init failed") << Exit;
    if( print )
    {
        f.PrintParams(cout);
        /*********************
        for( i=0; i<n_data; i++ )
        {
            double eta = f.up_data[i]/f.us_data[i];
            double c,s;
            if( f.Legendre(eta, c,s) )
                cerr << Error("f.Legendre failed") << Exit;
            double V = V0*(1-eta);
            double cref = sqrt(f.c2ref(V));
            cout << f.up_data[i] << " " << f.us_data[i]
                 << " " << C(eta) << " = " << cref
                 << ", c=" << c << ", s=" << s
                 << ", P=" << P(eta) << " " << f.Pref(V)
                 << "\n";
            

        }
        ******************/
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
        cout << up << " \t" << us << "\n";       
        /***
            double c,s;
            if( f.Legendre(eta, c,s) )
                cerr << Error("f.Legendre failed") << Exit;
            cout << up << " \t" << us
                 << ", c=" << c << ", s=" << s
                 <<", eta=" << eta
                 << "\n";
        ***/
    }

    return 0;
}
