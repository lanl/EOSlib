#ifndef EOSLIB_IDEAL_GAS_DETONATION_H
#define EOSLIB_IDEAL_GAS_DETONATION_H

#include <Wave.h>

class Detonation_IdealGas : public Detonation
{
private:	
	void operator=(const Detonation_IdealGas&);		    // disallowed
	Detonation_IdealGas(const Detonation_IdealGas&);    // disallowed
	Detonation_IdealGas *operator &();			        // disallowed
	IdealGas &gas;
    // set by InitCJ
    double gamma;
    double Cv;
    double delQ;        // energy released = Q/(P0*V0)
    int InitCJ();
    //double Detonation::{P0, V0, e0, u0};              // initial state
    //double Detonation::{Pcj, Dcj, Vcj, ucj, ecj};     // CJ wave
public:
	Detonation_IdealGas(IdealGas &Gas, double pvac = EOS::NaN);
	~Detonation_IdealGas();
	
	// find state on Detonation at specified P or u	
	int P(double p, int direction, WaveState &shock);
	int u(double u, int direction, WaveState &shock);
	int u_s(double us, int direction, WaveState &shock);
    int w_u_s(double us, int direction, WaveState &shock);
	int V(double V, int direction, WaveState &wave);
};

#endif // EOSLIB_IDEAL_GAS_DETONATION_H
