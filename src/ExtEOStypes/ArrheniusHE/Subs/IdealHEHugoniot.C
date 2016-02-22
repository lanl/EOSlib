#include "IdealHE.h"
#include "IdealHE_Hugoniot.h"

Hugoniot_IdealHE::Hugoniot_IdealHE(IdealHE &gas, double pvac)
                        : Hugoniot(gas,pvac)
{
    gamma   = gas.gamma;
    lambdaQ = gas.lambda_ref()*gas.Q;
}

Hugoniot_IdealHE::~Hugoniot_IdealHE()
{
	// Null
}

int Hugoniot_IdealHE::P(double p, int direction, WaveState &shock)
{
	double M2 = 1.+0.5*(gamma+1.)/gamma*(p/state0.P-1.);
	if( M2 < 1. )
		return -1;		
	shock.V  = (gamma-1. +2./M2)/(gamma+1.)*state0.V;
	shock.P  = p;
	shock.e  = p*shock.V/(gamma-1.)-lambdaQ;
	double us = direction*sqrt(M2)*state0.c;
	shock.us = us;
	shock.u  = state0.u + 2./(gamma+1.)*(1.-1./M2)*us;	
	return 0;
}

int Hugoniot_IdealHE::u_s(double us, int direction, WaveState &shock)
{
	double M = direction*(us-state0.u)/state0.c;
	if( M < 1. )
		return -1;
	double M2 = M*M;		
	shock.V  = (gamma-1.+2./M2)/(gamma+1.)*state0.V;
	shock.P  = (1.+2.*gamma/(gamma+1.)*(M2-1.))*state0.P;
	shock.e  = shock.P*shock.V/(gamma-1.)-lambdaQ;
	shock.us = us;
	shock.u  = state0.u+2./(gamma+1.)*(1.-1./M2)*(us-state0.u);	
	return 0;
}

int Hugoniot_IdealHE::V(double v, int direction, WaveState &shock)
{
    double den = (gamma+1.)*v -(gamma-1.)*state0.V;
	if( den <= 0 || state0.V < v)
		return -1;
	double M2 = 2.*state0.V/den;
	shock.V  = v;
	shock.P  = (1.+2.*gamma/(gamma+1.)*(M2-1.))*state0.P;
	shock.e  = shock.P*shock.V/(gamma-1.)-lambdaQ;
	double us = direction*sqrt(M2)*state0.c;
	shock.us = us;
	shock.u  = state0.u + 2./(gamma+1.)*(1.-1./M2)*us;
	return 0;
}

int Hugoniot_IdealHE::u(double u, int direction, WaveState &shock)
{
	double uc = direction*0.25*(gamma+1.)*(u-state0.u)/state0.c;	
	double M = uc +sqrt(1.+uc*uc);
    if( M < 1.)
        return -1;
	double M2 = M*M;
	shock.P  = (1.+2.*gamma/(gamma+1.)*(M2-1.))*state0.P;
	shock.V  = (gamma-1. +2./M2)/(gamma+1.)*state0.V;
	shock.e  = shock.P*shock.V/(gamma-1.)-lambdaQ;
	shock.u  = u;
	shock.us = state0.u + direction*M*state0.c;
	return 0;
}
