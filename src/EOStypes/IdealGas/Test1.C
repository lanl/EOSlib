#include <Arg.h>

#include <EOS.h>
#include "Isentrope_ODE.h"
#include "Hugoniot_gen.h"
#include "IdealGas.h"

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

// Test Isentrope

	cout.setf(ios::showpoint);
	
    Parameters param;
    param.base = Strdup("EOS");
    param.type = Strdup("IdealGas");
    param.name = Strdup("5/3");
    param.Append("gamma=5/3");
    param.Append("Cv=5e-5");
    param.Append("e_ref=1.5e-2");
    param.Append("V_ref=100");
 
	IdealGas gas;
    if( gas.EOS::Initialize(param) )
		cerr << Error("gas.Initialize failed") << Exit;
    gas.Print(cout);
    gas.P_vac = 1e-10;
	
	double V0 = 1;
	double e0 = 1;
	double u0 = 0;
	
	HydroState state0(V0,e0,u0);
	
	Isentrope *S1 = gas.isentrope(state0);
	Isentrope *S2 = new Isentrope_ODE(gas);
	if( S2->Initialize(state0) )
		cerr << Error("S2->Initialize failed") << Exit;

//	
	cerr << "\nTest V:\n\n";
	
	WaveStateLabel(cout) << "\n";

	double Vmin = 1e-6;
	double Vmax = 1e6;
	
	double V;
	for(V=Vmin; V < Vmax; V *= 10)
	{
		WaveState w1, w2;
		if( S1->V(V,dir,w1) )
			cerr << Error("S1->V failed") << Exit;
		
		if( S2->V(V,dir,w2) )
			cerr << Error("S2->V failed") << Exit;
		
		cout << w1 << "\n";
		cout << w2 << "\n";
		cout << "\n";
	}
	
//	
	cerr << "\nTest P:\n\n";
	
	WaveStateLabel(cout) << "\n";

	double Pmin = 1e-6;
	double Pmax = 1e6;
	
	double P;
	for(P=Pmin; P < Pmax; P *= 10)
	{
		WaveState w1, w2;
		if( S1->P(P,dir,w1) )
			cerr << Error("S1->P failed") << Exit;
		
		if( S2->P(P,dir,w2) )
			cerr << Error("S2->P failed") << Exit;
		
		cout << w1 << "\n";
		cout << w2 << "\n";
		cout << "\n";
	}

//	
	cerr << "\nTest u:\n\n";
	
	WaveStateLabel(cout) << "\n";

	double umin = -2.9;
	double umax = 2.9;
	
	double du = (umax-umin)/16;
	
	double u;
	for(u=umin; u < umax; u += du)
	{
		WaveState w1, w2;
		if( S1->u(u,dir,w1) )
			cerr << Error("S1->u failed") << Exit;
		
		if( S2->u(u,dir,w2) )
			cerr << Error("S2->u failed") << Exit;
		
		cout << w1 << "\n";
		cout << w2 << "\n";
		cout << "\n";
	}

//
	double u_right = S1->u_escape(RIGHT);
	double u_left = S1->u_escape(LEFT);
	cout << "S1, u(right) = " << u_right
	     << " \tu(left) = " << u_left << "\n";

	u_right = S2->u_escape(RIGHT);
	u_left = S2->u_escape(LEFT);
	cout << "S2, u(right) = " << u_right
	     << " \tu(left) = " << u_left << "\n";

	delete S1;
	delete S2;

// Test Hugoniot

	Hugoniot *H1 = gas.shock(state0);
	Hugoniot *H2 = new Hugoniot_generic(gas);

	if( H2->Initialize(state0) )
		cerr << Error("H2->Initialize failed") << Exit;

//
	cerr << "\nTest P:\n\n";
	
	WaveStateLabel(cout) << "\n";

	Pmin = H2->State0().P; //1e-6;
	Pmax = 1e6;
	
	for(P=Pmin; P < Pmax; P *= 10)
	{
		WaveState w1, w2;
		if( H1->P(P,dir,w1) )
			cerr << Error("H1->P failed") << Exit;
		
		if( H2->P(P,dir,w2) )
			cerr << Error("H2->P failed") << Exit;
		
		cout << w1 << "\n";
		cout << w2 << "\n";
		cout << "\n";
	}

//
	cerr << "\nTest u:\n\n";
	
	WaveStateLabel(cout) << "\n";

	umin = 0;
	umax = dir*100;
	
	du = (umax-umin)/16;
	for(u=umin; u/umax < 1; u += du)
	{
		WaveState w1, w2;
		if( H1->u(u,dir,w1) )
			cerr << Error("H1->u failed") << Exit;
		
		if( H2->u(u,dir,w2) )
			cerr << Error("H2->u failed") << Exit;
		
		cout << w1 << "\n";
		cout << w2 << "\n";

		double us = w2.us;
		if( H1->u_s(us,dir,w1) )
			cerr << Error("H1->u_s failed") << Exit;
		if( H2->u_s(us,dir,w2) )
			cerr << Error("H2->u_s failed") << Exit;
		cout << w1 << "\n";
		cout << w2 << "\n";

		cout << "\n";
	}

	delete H1;
	delete H2;
	
	
	
	return 0;
}
