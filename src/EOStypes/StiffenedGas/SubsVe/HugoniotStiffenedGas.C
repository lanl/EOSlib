#include <EOS.h>
#include "StiffenedGas.h"

class Hugoniot_StiffenedGas : public Hugoniot
{
private:	
	void operator=(const Hugoniot_StiffenedGas&);		// disallowed
	Hugoniot_StiffenedGas(const Hugoniot_StiffenedGas&);		// disallowed
	Hugoniot_StiffenedGas *operator &();			// disallowed
	StiffenedGas &gas;
public:
	Hugoniot_StiffenedGas(StiffenedGas &Gas, double pvac=EOS::NaN);
	~Hugoniot_StiffenedGas();
	
	// find state on Hugoniot at specified P or u	
	int P(double p, int direction, WaveState &shock);
	int u(double u, int direction, WaveState &shock);
	int u_s(double us, int direction, WaveState &shock);
	int V(double V, int direction, WaveState &wave);
};

Hugoniot_StiffenedGas::Hugoniot_StiffenedGas(StiffenedGas &Gas, double pvac)
            : Hugoniot(Gas,pvac), gas(Gas)
{
    // Null
}

Hugoniot_StiffenedGas::~Hugoniot_StiffenedGas()
{
	// Null
}

int Hugoniot_StiffenedGas::P(double p, int direction, WaveState &shock)
{
	double g = gas.G+1;
	double P_p = gas.P_p;
		
	double M2 = 1 + 0.5*(g+1)/g*((p+P_p)/(state0.P+P_p) - 1);
	if( M2 < 1 )
		return -1;
		
	shock.P  = p;
	shock.V  = (g-1 +2/M2)/(g+1)*state0.V;
	shock.e  = state0.e + 0.5*(state0.P + shock.P)*(state0.V-shock.V);
	double us = direction*sqrt(M2)*state0.c;
	shock.us = state0.u + us;
	shock.u  = state0.u + 2/(g+1)*(1-1/M2)*us;
	
	return 0;
}

int Hugoniot_StiffenedGas::u_s(double us, int direction, WaveState &shock)
{
	double g = gas.G+1;
	double P_p = gas.P_p;
		
	double M = direction*(us-state0.u)/state0.c;
	if( M < 1 )
		return -1;
	double M2 = M*M;
		
	shock.P  = (1+2*g/(g+1)*(M2-1))*(state0.P+P_p) - P_p;
	shock.V  = (g-1 +2/M2)/(g+1)*state0.V;
	shock.e  = state0.e + 0.5*(state0.P + shock.P)*(state0.V-shock.V);
	shock.us = us;
	shock.u  = state0.u + 2/(g+1)*(1-1/M2)*(us-state0.u);
	
	return 0;
}

int Hugoniot_StiffenedGas::V(double v, int direction, WaveState &shock)
{
	double g = gas.G+1;
	double P_p = gas.P_p;		
    double den = (g+1)*v -(g-1)*state0.V;
	if( den <= 0 || state0.V < v)
		return -1;
	double M2 = 2*state0.V/den;

	shock.P  = (1+2*g/(g+1)*(M2-1))*(state0.P+P_p) - P_p;
	shock.V  = v;
	shock.e  = state0.e + 0.5*(state0.P + shock.P)*(state0.V-shock.V);
	double us = direction*sqrt(M2)*state0.c;
	shock.us = us;
	shock.u  = state0.u + 2/(g+1)*(1-1/M2)*us;
	
	return 0;
}

int Hugoniot_StiffenedGas::u(double u, int direction, WaveState &shock)
{
	double g = gas.G+1;
	double P_p = gas.P_p;
	
	double uc = direction*0.25*(g+1)*(u-state0.u)/state0.c;
	
	double M = uc +sqrt(1+uc*uc);
    if( M < 1)
        return -1;
	double M2 = M*M;
		
	shock.P  = (1+2*g/(g+1)*(M2-1))*(state0.P+P_p) - P_p;
	shock.V  = (g-1 +2/M2)/(g+1)*state0.V;
	shock.e  = state0.e + 0.5*(state0.P + shock.P)*(state0.V-shock.V);
	shock.u  = u;
	shock.us = state0.u + direction*M*state0.c;
	
	return 0;
}

Hugoniot *StiffenedGas::shock(const HydroState &state)
{
	Hugoniot *H = new Hugoniot_StiffenedGas(*this);
	int status = H->Initialize(state);
	
	if( status )
	{
	    EOSerror->Log("StiffenedGas::shock", __FILE__, __LINE__, this,
	                   "Initialize failed with status %d\n", status);
	    delete H;
	    H = NULL;
	}

	return H;
}

