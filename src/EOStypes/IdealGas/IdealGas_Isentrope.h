#ifndef EOSLIB_IDEAL_GAS_ISENTROPE_H
#define EOSLIB_IDEAL_GAS_ISENTROPE_H

#include <Wave.h>

class Isentrope_IdealGas : public Isentrope
{
private:
	Isentrope_IdealGas(EOS &Gas);				// disallowed
	void operator=(const Isentrope_IdealGas&);		// disallowed
	Isentrope_IdealGas(const Isentrope_IdealGas&);		// disallowed
	Isentrope_IdealGas *operator &();			// disallowed
	
	IdealGas &gas;
public:
	Isentrope_IdealGas(IdealGas &Gas, double p = EOS::NaN);
	~Isentrope_IdealGas();
	
	// find state on isentrope at specified V, P or u
	int V(double V, int direction, WaveState &wave);
	int P(double p, int direction, WaveState &wave);
	int u(double u, int direction, WaveState &wave);
	int u_s(double us, int direction, WaveState &wave);
	
	double u_escape(int direction);
	int u_escape(int direction, WaveState &wave);
};


#endif // EOSLIB_IDEAL_GAS_ISENTROPE_H

