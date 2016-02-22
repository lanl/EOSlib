#include <LocalMath.h>
#include <Arg.h>

#include <EOS.h>
#include <IO.h>
#include "UsUp.h"

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
    param.type = Strdup("UsUp");
    param.name = Strdup("HMX");
    param.Append("rho0=1.9");
    param.Append("P0=1.0e-4");
    param.Append("T0=300");
    param.Append("c0=2.6");
    param.Append("s=2.5");
    param.Append("Cv=1.5e-3");
    param.Append("G0=1.1");
    param.Append("G1=1.1");
 
 
	UsUp solid;
    if( solid.EOS::Initialize(param) )
		cerr << Error("solid.Initialize failed") << Exit;
    solid.Print(cout);
    solid.P_vac = solid.Pmin;
	
    double s    = solid.s;
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
		cout << w1 << " us=" << c0+s*w1.u
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
	double Vmin = 0.5*V0*(2. - 1./s);
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
    Format S_form;
    T_form.width = 7;
    V_form.width = 7;
    u_form.width = 7;
    cout << Center(V_form,"V") << " "
         << Center(e_form,"e") << " "
         << Center(P_form,"P") << " "
         << Center(T_form,"T") << " "
         << Center(S_form,"S") << " "
         << Center(P_form,"Pref") << " "
         << Center(T_form,"Tref") << " "
         << Center(c_form,"cref") << " "
         << Center(P_form,"dPref") << " "
         << "\n";
	V = V0;	
    f = pow(Vmax/V0, 1./Nsteps);
	for( i=0; i <nsteps; i++, V *=f )
	{
        double Pref = solid.Pref(V);
        double eref = solid.eref(V);
        double Tref = solid.Tref(V);
        double Sref = solid.S(V,eref);
        double cref = sqrt(solid.c2ref(V));
		WaveState w1;
		if( S1->V(V,dir,w1) )
			cerr << Error("S1->V failed at V=") << V << Exit;
		
		cout << V_form << w1.V << " "
             << e_form << w1.e << " "
             << P_form << w1.P << " "
             << T_form << solid.EOS::T(w1) << " "
             << S_form << solid.EOS::S(w1) << " "
             << P_form << Pref << " "
             << T_form << Tref << " "
             << u_form << cref << " "
             << u_form << solid.dPref(V) << " "
             << "\n";
	}
	delete S1;
	return 0;
}
