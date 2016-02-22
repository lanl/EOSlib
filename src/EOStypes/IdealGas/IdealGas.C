#include <Arg.h>

#include <EOS.h>
#include <Riemann.h>
#include "IdealGas.h"

int main(int, char **argv)
{
	ProgName(*argv);
	
	double gamma = 5./3.;
	double V = 1;
	double e = 1;
	double u = 0.0;
	
	int nsteps = 10;
	double Pmax = 10;
	int dir = RIGHT;
	
	while(*++argv)
	{
		GetVar(gamma,gamma);
		GetVar(V,V);
		GetVar(e,e);
		GetVar(u,u);
		
		GetVarValue(left,dir,LEFT);
		GetVarValue(right,dir,RIGHT);
		
		GetVar(nsteps,nsteps);
		GetVar(Pmax,Pmax);
		
		ArgError;
	}

	cout.setf(ios::showpoint);
	
	IdealGas gas(gamma);
	
	HydroState state0(V,e,u);
	HydroThermal s0;
	
	gas.Evaluate(state0,s0);
	double P0 = s0.P;
	
	HydroThermalLabel(cout) << "\n";
	cout << s0 << "\n";	

	
	Wave *wave;	
	if( Pmax > P0 )
		wave = gas.shock(state0);
	else
		wave = gas.isentrope(state0);
	
	double P;
	double dP = (Pmax-P0)/nsteps;
	int count;
	WaveState waveP;
		
	WaveStateLabel(cout) << "\n";
	for( P=P0, count=nsteps+1; count--; P += dP)
	{
		if( wave->P(P,dir,waveP) )
			cerr << Error("wave->P failed") << Exit;
		cout << waveP << "\n";
	}
	
	delete wave;
	return 0;
}
