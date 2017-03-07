#include <Arg.h>
#include <iostream>
#include "Spline.h"
using namespace std;
class PhiEq : public SplineFunction
{
public:
	double P0;
	double V0;
	
	double phi0;
	double Pc;
	
	PhiEq(double P_0 = 1e-6, double V_0 = 0.5,
		double phi_0 = 0.64, double P_c = 1e-3)
		: P0(P_0), V0(V_0), phi0(phi_0), Pc(P_c) { }
	~PhiEq() { }

	double f(double PV);

};


double PhiEq::f(double PV)
{
	double r = (PV - P0*V0)/(Pc*V0);
	return 1 -(1-phi0)*exp(-r);
}

int main(int, char **argv)
{
	ProgName(*argv);
	
	double abs_tol = 1e-3;
	double rel_tol = 1e-3;
	
	int test_evaluate = 0;
	int show_knots = 1;
	int test_inverse = 1;
	
	while(*++argv)
	{
		GetVarValue(test_evaluate, test_evaluate,1);
		GetVarValue(show_knots, show_knots,1);
		GetVarValue(test_inverse, test_inverse,1);
		
		GetVar(abs_tol,abs_tol);
		GetVar(rel_tol,rel_tol);
		ArgError;
	}
	
	PhiEq phi_eq;
	phi_eq.abs_tol = abs_tol;
	phi_eq.rel_tol = rel_tol;
	
	double PV_0 = phi_eq.P0 * phi_eq.V0;
	double PV_1 = 10e-3 * phi_eq.V0;
	phi_eq.init(PV_0, PV_1, NaN, NaN);
	
	int N = phi_eq.Nknot();
	
	cerr <<  N << " knots\n";
	
	int nsteps = 100;
	
	double PV = PV_0;
	double dPV = (PV_1 - PV_0)/nsteps;
	
	int i;
	
	cout << ".line\n";
	for( i=0; i<=nsteps; i++, PV += dPV)
		cout << PV << " " << phi_eq.f(PV) << "\n";
	
	if( test_evaluate )
	{
		double sum = 0;
		cout << ".points symbol x\n";
		for( i=0, PV= PV_0; i<=nsteps; i++, PV += dPV)
		{
			cout << PV << " " << phi_eq.Evaluate(PV) << "\n";
			double df = phi_eq.Evaluate(PV) - phi_eq.f(PV);
			sum += df*df;
		}
		cerr << "rms error = " << sqrt(sum/nsteps) << "\n";
	}
		
	if( test_inverse )
	{
		double phi_c = 1-phi_eq.f_k(N-1);
		
		double factor = exp( log( (1-phi_eq.phi0)/phi_c )/nsteps );
		//double dphi = (phi_eq.f_k(N-1)-phi)/nsteps;
		
		int count = 0;
	
		cout << ".points symbol x\n";
		for( i=0; i<=nsteps; i++, phi_c *= factor)
		{
			double phi = 1- phi_c;
			phi = min(phi_eq.f_k(N-1),max(phi_eq.f_k(0),phi));
			cout << phi_eq.Inverse(phi) << " " << phi << "\n";
			count += phi_eq.count();
		}
		
		cerr << "iteration per inverse = " 
		     << (double)count/nsteps << "\n";
	
	}

	if( show_knots )
	{	
		cout << ".points symbol open_circle\n";
		for( i=0; i<N; i++)
			cout << phi_eq.x_k(i) << " " << phi_eq.f_k(i) << "\n";
	}
	
	return 0;
}
