#include <Arg.h>

#include <EOS.h>
#include "BirchMurnaghan.h"

int main(int, char **argv)
{
	ProgName(*argv);
	
	double abs_tol = 1e-4;
	double rel_tol = 1e-4;
	
	int nsteps = 10;
	
	double PV_ratio = 10.;
	double e_ratio = 1;
	
	int dir = RIGHT;
	
	while(*++argv)
	{
		GetVar(abs_tol,abs_tol);
		GetVar(rel_tol,rel_tol);
		
		GetVar(PV_ratio, PV_ratio);
		GetVar(e_ratio, e_ratio);

		GetVar(nsteps,nsteps);
		
		GetVarValue(left,dir,LEFT);
		GetVarValue(right,dir,RIGHT);
		
		ArgError;
	}
	cout.setf(ios::showpoint);

    Parameters param;
    param.base = Strdup("EOS");
    param.type = Strdup("BirchMurnaghan");
    param.name = Strdup("HMX");
    param.Append("T_ref = 300.");
    param.Append("P_ref = 1.e-4");
    param.Append("V0  =  0.511509");
    param.Append("e0  =  0");
    param.Append("K0  = 17.66");
    param.Append("Kp0 =  8.5");
    param.Append("b   = -0.25");
    param.Append("a   =  1.20");
    param.Append("cv(1)=5.2653675003188e-1, 3.0733581008896e2,"    
                 "1.8318931090834e5, 4.1941409666532e2");    
 
 
	BirchMurnaghan solid;
    if( solid.EOS::Initialize(param) )
		cerr << Error("solid.Initialize failed") << Exit;
    solid.Print(cout);
    solid.P_vac = 1e-10;
	
	double V0 = solid.V_ref;
	double e0 = solid.e_ref;
	double u0 = 0;
    double c0 = solid.c(V0,e0);
	HydroState state0(V0,e0,u0);
	
// Test Isentrope
	Isentrope *S1 = solid.isentrope(state0);
	WaveState w1;
//	
	cout << "\nTest V:\n\n";
	WaveStateLabel(cout) << "\n";

	double Vmin = 0.6*V0;
	double Vmax = V0;
	
	double V;
	for(V=Vmin; V < Vmax; V *= 1.1)
	{
		if( S1->V(V,dir,w1) )
			cerr << Error("S1->V failed") << Exit;
		cout << w1 << "\n";
	}
//	
	cout << "\nTest P:\n\n";
	WaveStateLabel(cout) << "\n";

	double Pmin = 0.001;
	double Pmax = 1000;	
	double P;
	for(P=Pmin; P < Pmax; P *= 10)
	{
		if( S1->P(P,dir,w1) )
			cerr << Error("S1->P failed") << Exit;
		cout << w1 << "\n";
	}
//	
	cout << "\nTest u:\n\n";	
	WaveStateLabel(cout) << "\n";

	double umin = 0;
	double umax = 2*c0;	
	double du = (umax-umin)/10;	
	double u;
	for(u=umin; u < umax; u += du)
	{
		if( S1->u(u,dir,w1) )
			cerr << Error("S1->u failed") << Exit;
		cout << w1 << "\n";
	}
//
	double u_right = S1->u_escape(RIGHT);
	double u_left = S1->u_escape(LEFT);
	cout << "S1, u(right) = " << u_right
	     << " \tu(left) = " << u_left << "\n";

	delete S1;

// Test Hugoniot
	Hugoniot *H1 = solid.shock(state0);
//
	cout << "\nTest P:\n\n";	
	WaveStateLabel(cout) << "\n";

	Pmin = H1->State0().P;;
	Pmax = 1000;	
	for(P=Pmin; P < Pmax; P *= 2)
	{
		if( H1->P(P,dir,w1) )
			cerr << Error("H1->P failed") << Exit;
		cout << w1 << "\n";
	}
//
	cout << "\nTest u:\n\n";	
	WaveStateLabel(cout) << "\n";

	umin = 0;
	umax = dir*100;	
	du = (umax-umin)/10;
	for(u=umin; u/umax < 1; u += du)
	{
		if( H1->u(u,dir,w1) )
			cerr << Error("H1->u failed") << Exit;
		cout << w1 << "\n";

		double us = w1.us;
		if( H1->u_s(us,dir,w1) )
			cerr << Error("H1->u_s failed") << Exit;
		cout << w1 << "\n";
		cout << "\n";
	}

	delete H1;
	
	return 0;
}
