#include <EOS.h>
#include "IdealGas.h"
#include "IdealGas_Hugoniot.h"

Hugoniot *IdealGas::shock(const HydroState &state)
{
	Hugoniot *H = new Hugoniot_IdealGas(*this);
	int status = H->Initialize(state);
	if( status )
	{
	    EOSerror->Log("IdealGas::shock", __FILE__, __LINE__, this,
	                  "Initialize failed with status %d\n", status);
	    delete H;
	    H = NULL;
	}
	return H;
}

//

Hugoniot_IdealGas::Hugoniot_IdealGas(IdealGas &Gas, double pvac)
                        : Hugoniot(Gas,pvac), gas(Gas)
{
    // Null
}

Hugoniot_IdealGas::~Hugoniot_IdealGas()
{
	// Null
}

int Hugoniot_IdealGas::P(double p, int dir, WaveState &shock)
{
	double g = gas.gamma;
	double M2 = 1 + 0.5*(g+1)/g*(p/state0.P - 1);
	if( M2 < 1 )
		return -1;
		
	shock.V  = (g-1 +2/M2)/(g+1)*state0.V;
	shock.P  = p;
	shock.e  = p*shock.V/(g-1);
	double us = dir*sqrt(M2)*state0.c;
	shock.us = us;
	shock.u  = state0.u + 2/(g+1)*(1-1/M2)*us;
	
	return 0;
}

int Hugoniot_IdealGas::u_s(double us, int dir, WaveState &shock)
{
	double g = gas.gamma;
	double M = dir*(us-state0.u)/state0.c;
	if( M < 1 )
		return -1;
	double M2 = M*M;
		
	shock.V  = (g-1 +2/M2)/(g+1)*state0.V;
	shock.P  = (1+2*g/(g+1)*(M2-1))*state0.P;
	shock.e  = shock.P*shock.V/(g-1);
	shock.us = us;
	shock.u  = state0.u + 2/(g+1)*(1-1/M2)*(us-state0.u);
	
	return 0;
}

int Hugoniot_IdealGas::V(double v, int dir, WaveState &shock)
{
	double g = gas.gamma;
    double den = (g+1)*v -(g-1)*state0.V;
	if( den <= 0 || state0.V < v)
		return -1;
	double M2 = 2*state0.V/den;
		
	shock.V  = v;
	shock.P  = (1+2*g/(g+1)*(M2-1))*state0.P;
	shock.e  = shock.P*shock.V/(g-1);
	double us = dir*sqrt(M2)*state0.c;
	shock.us = us;
	shock.u  = state0.u + 2/(g+1)*(1-1/M2)*us;
	
	return 0;
}

int Hugoniot_IdealGas::u(double u, int dir, WaveState &shock)
{
	double g = gas.gamma;	
	double uc = dir*0.25*(g+1)*(u-state0.u)/state0.c;	
	double M = uc +sqrt(1+uc*uc);
    if( M < 1)
        return -1;
	double M2 = M*M;
		
	shock.P  = (1+2*g/(g+1)*(M2-1))*state0.P;
	shock.V  = (g-1 +2/M2)/(g+1)*state0.V;
	shock.e  = shock.P*shock.V/(g-1);
	shock.u  = u;
	shock.us = state0.u + dir*M*state0.c;
	
	return 0;
}
