#ifndef EOSLIB_IDEAL_GAS_DEFLAGRATION_H
#define EOSLIB_IDEAL_GAS_DEFLAGRATION_H

#include <Wave.h>

class Deflagration_IdealGas : public Deflagration
{
private:	
	void operator=(const Deflagration_IdealGas&);		    // disallowed
	Deflagration_IdealGas(const Deflagration_IdealGas&);    // disallowed
	Deflagration_IdealGas *operator &();			        // disallowed
	IdealGas &gas;
    // set by InitCJ
    double gamma;
    double Cv;
    double delQ;        // energy released = Q/(P0*V0)
    int InitCJ();
    //double Deflagration::{P0, V0, e0, u0};              // initial state
    //double Deflagration::{Vp0, ep0};                    // wave at P=P0
    //double Deflagration::{Pcj, Dcj, Vcj, ucj, ecj};     // CJ wave
public:
	Deflagration_IdealGas(IdealGas &Gas, double pvac = EOS::NaN);
	~Deflagration_IdealGas();	
	// find state on Deflagration at specified P or u	
	int P(double p, int direction, WaveState &shock);
	int u(double u, int direction, WaveState &shock);
	int u_s(double us, int direction, WaveState &shock);
	int V(double V, int direction, WaveState &wave);
};

#endif // EOSLIB_IDEAL_GAS_DEFLAGRATION_H
