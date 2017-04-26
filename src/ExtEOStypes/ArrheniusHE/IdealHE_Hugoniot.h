#ifndef EOSLIB_IDEAL_HE_HUGONIOT_H
#define EOSLIB_IDEAL_HE_HUGONIOT_H

#include <Wave.h>

class Hugoniot_IdealHE : public Hugoniot
{
private:	
    double gamma;
    double lambdaQ;
	void operator=(const Hugoniot_IdealHE&);		// disallowed
	Hugoniot_IdealHE(const Hugoniot_IdealHE&);		// disallowed
	Hugoniot_IdealHE *operator &();			        // disallowed
public:
	Hugoniot_IdealHE(IdealHE &gas, double pvac = EOS::NaN);
	~Hugoniot_IdealHE();
	// find state on Hugoniot at specified P or u	
	int P(double p, int direction, WaveState &shock);
	int u(double u, int direction, WaveState &shock);
	int u_s(double us, int direction, WaveState &shock);
	int V(double V, int direction, WaveState &wave);
};

class Detonation_IdealHE : public Detonation
{
private:	
    double gamma;
    double lambda0;
    double Q;
    double Cv;
    // set by InitCJ
    double delQ;        // energy released = (1-lambda0)*Q/(P0*V0)
    int InitCJ();
	void operator=(const Detonation_IdealHE&);      // disallowed
	Detonation_IdealHE(const Detonation_IdealHE&);  // disallowed
	Detonation_IdealHE *operator &();			    // disallowed
public:
	Detonation_IdealHE(IdealHE &Gas, double pvac = EOS::NaN);
	~Detonation_IdealHE();
	// find state on Detonation at specified P or u	
	int P(double p, int direction, WaveState &shock);
	int u(double u, int direction, WaveState &shock);
	int u_s(double us, int direction, WaveState &shock);
	int w_u_s(double us, int direction, WaveState &shock);
	int V(double V, int direction, WaveState &wave);
};

class Deflagration_IdealHE : public Deflagration
{
private:	
    double gamma;
    double lambda0;
    double Q;
    double Cv;
    // set by InitCJ
    double delQ;        // energy released = (1-lambda0)*Q/(P0*V0)
    int InitCJ();
	void operator=(const Deflagration_IdealHE&);        // disallowed
	Deflagration_IdealHE(const Deflagration_IdealHE&);  // disallowed
	Deflagration_IdealHE *operator &();			        // disallowed
public:
	Deflagration_IdealHE(IdealHE &Gas, double pvac = EOS::NaN);
	~Deflagration_IdealHE();
	// find state on Deflagration at specified P or u	
	int P(double p, int direction, WaveState &shock);
	int u(double u, int direction, WaveState &shock);
	int u_s(double us, int direction, WaveState &shock);
	int V(double V, int direction, WaveState &wave);
};

#endif // EOSLIB_IDEAL_HE_HUGONIOT_H
