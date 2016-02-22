#include <LocalMath.h>
#include <Arg.h>

#include <EOS.h>
#include "UsUpSpline.h"

int main(int, char **argv)
{
	ProgName(*argv);
	
	int nsteps = 10;
	int dir = RIGHT;
    double umin = 0.;
    double umax = 0;
	
	while(*++argv)
	{
		GetVar(nsteps,nsteps);
		GetVarValue(left,dir,LEFT);
		GetVarValue(right,dir,RIGHT);
		
		ArgError;
	}
	cout.setf(ios::showpoint);

    Parameters param;
    param.base = Strdup("EOS");
    param.type = Strdup("UsUpSpline");
    param.name = Strdup("PMMA");
    param.Append("rho0=1.185");
    param.Append("P0=1.0e-4");
    param.Append("T0=300");
    param.Append("Cv=1.e-3");
    param.Append("G0=G1=0.5");
    param.Append("s1=1.54");
    param.Append("dusdup0=3.89798");
    param.Append("up_data(1)= 0.0,      0.0585621, 0.12168,  0.186838, 0.252758");
	param.Append("us_data(1)= 2.75267,  2.92811,   3.042,    3.11396,  3.15948");
    param.Append("up_data(6)= 0.319218, 0.386981,  0.457749, 0.534085, 0.619237");
    param.Append("us_data(6)= 3.19218,  3.22484,   3.26964,  3.33803,  3.4402");     
 
	UsUpSpline solid;
    if( solid.EOS::Initialize(param) )
		cerr << Error("solid.Initialize failed") << Exit;
    solid.Print(cout);
    solid.P_vac = solid.Pmin;
	
    double V1 = solid.V1;
    double Vmax = solid.Vmax;
    
	double V0 = solid.V_ref;
	double e0 = solid.e_ref;
	double u0 = 0;
    double c0 = solid.c(V0,e0);
	HydroState state0(V0,e0,u0);
	
    int i;
	double Nsteps = (double) nsteps;
// Test Hugoniot
	Hugoniot *H1 = solid.shock(state0);
	if( H1 == NULL )
		cerr << Error("H1 is NULL") << Exit;
	cout << "\nTest Hugoniot\n";	
	WaveStateLabel(cout) << "\n";
    if( umax <= 0 )
        umax = 2*c0;
    for(i=0; i<=nsteps; i++ )
	{
        double u = umin + (i/Nsteps)*(umax-umin);
		WaveState w1;
		if( H1->u(u,dir,w1) )
			cerr << Error("H1->u failed") << Exit;
        double eta = w1.u/w1.us;
		cout << w1 << " us=" << solid.Us(eta)
             << " T=" << solid.EOS::T(w1) 
             << " S=" << solid.EOS::S(w1) 
             << "\n";
	}
	delete H1;
// Test Isentrope
	Isentrope *S1 = solid.isentrope(state0);
	if( S1 == NULL )
		cerr << Error("S1 is NULL") << Exit;	
    double V, f;
// Compressive branch
    cout << "\nTest Isentrope (compressive branch)\n";
	WaveStateLabel(cout) << "\n";
	double Vmin = 0.5*(V0+V1);
	V = V0;	
    f = pow(Vmin/V0, 1./Nsteps);
	for( i=0; i <=nsteps; i++, V *=f )
	{
		WaveState w1;
		if( S1->V(V,dir,w1) )
			cerr << Error("S1->V failed") << Exit;
		
		cout << w1
             << " T=" << solid.EOS::T(w1)
             << " S=" << solid.EOS::S(w1)
             << "\n";
	}
// Expansive branch
	cout << "\nTest Isentrope (expansive branch)\n";
	WaveStateLabel(cout) << "\n";
	V = V0;	
    f = pow(Vmax/V0, 1./Nsteps);
	for( i=0; i <nsteps; i++, V *=f )
	{
		WaveState w1;
		if( S1->V(V,dir,w1) )
			cerr << Error("S1->V failed at V=") << V << Exit;
		
		cout << w1
             << " T=" << solid.EOS::T(w1)
             << " S=" << solid.EOS::S(w1)
             << " c=" << solid.EOS::c(w1)
             << "\n";
	}
	delete S1;
	return 0;
}
