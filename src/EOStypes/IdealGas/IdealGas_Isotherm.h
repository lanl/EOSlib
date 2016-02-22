#ifndef EOSLIB_IDEAL_GAS_ISOTHERM_H
#define EOSLIB_IDEAL_GAS_ISOTHERM_H

#include <Wave.h>

class Isotherm_IdealGas : public Isotherm
{
private:
	Isotherm_IdealGas();				            // disallowed
	Isotherm_IdealGas(const Isotherm_IdealGas&);	// disallowed
	void operator=(const Isotherm_IdealGas&);	    // disallowed
	Isotherm_IdealGas *operator &();		        // disallowed
	
	IdealGas &gas;
public:
	Isotherm_IdealGas(IdealGas &Gas, double pvac = EOS::NaN); 
	~Isotherm_IdealGas();	

	int P(double p, ThermalState &state);
	int V(double v, ThermalState &state);
};

#endif // EOSLIB_IDEAL_GAS_ISOTHERM_H

