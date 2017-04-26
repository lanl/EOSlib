#include <Riemann.h>

#ifndef _IDEAL_GAS_RIEMANN_H
#define _IDEAL_GAS_RIEMANN_H

class RiemannSolver_IdealGas : public RiemannSolver
{
private:	
	RiemannSolver_IdealGas();				// disallowed
	RiemannSolver_IdealGas(const RiemannSolver_IdealGas&);	// disallowed
	void operator=(const RiemannSolver_IdealGas&);		// disallowed
	RiemannSolver_IdealGas *operator &();			// disallowed
	
	IdealGas &gas_l;
	IdealGas &gas_r;
public:
	double RelErr;		// default: 1e-6
	int max_iter;		// default: 25
	double Pvac;		// cutoff pressure for vacuum: default 1e-6

	RiemannSolver_IdealGas(IdealGas &left, IdealGas &right)
		: RiemannSolver(left,right), gas_l(left), gas_r(right),
		  RelErr(1e-6), max_iter(25), Pvac(1e-6) { }
	~RiemannSolver_IdealGas();
	int Solve(const HydroState &state_l, const HydroState &state_r,
		  WaveState &left,     WaveState &right);
};

#endif // _IDEAL_GAS_RIEMANN_H

