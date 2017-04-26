#ifndef EOSLIB_IDEAL_GAS_HUGONIOT_H
#define EOSLIB_IDEAL_GAS_HUGONIOT_H

#include <Wave.h>


class Hugoniot_IdealGas : public Hugoniot
{
private:	
	void operator=(const Hugoniot_IdealGas&);		// disallowed
	Hugoniot_IdealGas(const Hugoniot_IdealGas&);		// disallowed
	Hugoniot_IdealGas *operator &();			// disallowed
	IdealGas &gas;
public:
	Hugoniot_IdealGas(IdealGas &Gas, double pvac = EOS::NaN);
	~Hugoniot_IdealGas();
	
	// find state on Hugoniot at specified P or u	
	int P(double p, int direction, WaveState &shock);
	int u(double u, int direction, WaveState &shock);
	int u_s(double us, int direction, WaveState &shock);
	int V(double V, int direction, WaveState &wave);
};

#endif // EOSLIB_IDEAL_GAS_HUGONIOT_H

