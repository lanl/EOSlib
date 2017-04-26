#ifndef EOSLIB_IDEAL_HE_ISOTHERM_H
#define EOSLIB_IDEAL_HE_ISOTHERM_H

#include <Wave.h>

class Isotherm_IdealHE : public Isotherm
{
private:
	double gamma_m;  // gamma-1
    double lambdaQ;  // lambda*Q
    double e0;       // state0.e + lambda*Q
    
	Isotherm_IdealHE();				                // disallowed
	Isotherm_IdealHE(const Isotherm_IdealHE&);	    // disallowed
	void operator=(const Isotherm_IdealHE&);	    // disallowed
	Isotherm_IdealHE *operator &();		            // disallowed
public:
	Isotherm_IdealHE(IdealHE &gas, double pvac = EOS::NaN); 
	~Isotherm_IdealHE();	
    int Initialize(const HydroState &state);

	int P(double p, ThermalState &state);
	int V(double v, ThermalState &state);
};

#endif // EOSLIB_IDEAL_HE_ISOTHERM_H
