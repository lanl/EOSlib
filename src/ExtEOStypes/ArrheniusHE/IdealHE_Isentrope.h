#ifndef EOSLIB_IDEAL_HE_ISENTROPE_H
#define EOSLIB_IDEAL_HE_ISENTROPE_H

#include <Wave.h>

class Isentrope_IdealHE : public Isentrope
{
private:
    double gamma;
    double lambdaQ;
    double e0;
	Isentrope_IdealHE(EOS &gas);				    // disallowed
	void operator=(const Isentrope_IdealHE&);		// disallowed
	Isentrope_IdealHE(const Isentrope_IdealHE&);    // disallowed
	Isentrope_IdealHE *operator &();			    // disallowed	
public:
	Isentrope_IdealHE(IdealHE &gas, double p = EOS::NaN);
	~Isentrope_IdealHE();
    int Initialize(const HydroState &state);
	
	// find state on isentrope at specified V, P or u
	int V(double V, int direction, WaveState &wave);
	int P(double p, int direction, WaveState &wave);
	int u(double u, int direction, WaveState &wave);
	int u_s(double us, int direction, WaveState &wave);
	
	double u_escape(int direction);
	int u_escape(int direction, WaveState &wave);
};

#endif // EOSLIB_IDEAL_HE_ISENTROPE_H
