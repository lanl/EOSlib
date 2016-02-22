#include "EOS.h"
#include "IdealGas.h"
#include "IdealGas_Deflagration.h"

Deflagration *IdealGas::deflagration(const HydroState &state, double p0)
{
	Deflagration *H = new Deflagration_IdealGas(*this);
	if( H->Initialize(state, p0) )
	{
	    EOSerror->Log("IdealGas::deflagration", __FILE__, __LINE__, this,
	                  "Initialize failed\n");
	    delete H;
	    H = NULL;
	}
	return H;
}

//


Deflagration_IdealGas::Deflagration_IdealGas(IdealGas &Gas, double pvac)
                        : Deflagration(Gas,pvac), gas(Gas)
{
    delQ  = 0.0;
}

Deflagration_IdealGas::~Deflagration_IdealGas()
{
	// Null
}

int Deflagration_IdealGas::InitCJ()
{
    gamma = gas.gamma;
    Cv    = gas.Cv;
    double p = (gamma-1.)*e0/V0;
    if( P0 >= p || gamma <- 1. || Cv <= 0. )
        return 1;
    delQ = (p/P0-1.);
    state0.c = sqrt(gamma*P0*V0);
    state0.T = P0*V0/(gamma-1.)/Cv;

    Vp0 = (gamma-1.)/gamma *(e0/P0 + V0);
    ep0 = P0*Vp0/(gamma-1.);

    double b = 1.+delQ;
    double c = 1.+2.*delQ/(gamma+1.);
    Pcj = (b - sqrt(b*b-c))*P0;
    double dP = P0-Pcj;
    Vcj = gamma*Pcj/(gamma*Pcj-dP)*V0;
    ecj = Pcj*Vcj/(gamma-1.);
    Dcj = sqrt(gamma*Pcj/Vcj)*V0;
    ucj = (1.-Vcj/V0)*Dcj;
    return 0;    
}

int Deflagration_IdealGas::P(double p, int dir, WaveState &shock)
{
    if(  p < 0.9999*Pcj || 1.0001*P0 < p )
        return -1;
	if( p < Pcj  )
		p = Pcj;
    else if( p > 0.9999*P0 )
    {
        shock.V = Vp0;
        shock.e = ep0;
        shock.P = P0;
        shock.u = u0;
        shock.us = u0;
        return 0;
    }
    double dp = (p-P0)/P0;
    double M2 = (0.5*(gamma+1.)*dp/gamma+1.)/(1.-delQ/dp);
    double epsilon = dp/(gamma*M2);
	shock.V  = (1. - epsilon)*state0.V;
	shock.P  = p;
	shock.e  = p*shock.V/(gamma-1.);
	double us = dir*sqrt(M2)*state0.c;
	shock.us = u0 + us;
    shock.u  = u0 + epsilon*us;
	return 0;
}

int Deflagration_IdealGas::u_s(double us, int dir, WaveState &shock)
{
	shock.us = us;
    us = dir*(us-u0);
    if( us < -0.0001*Dcj || 1.0001*Dcj < us )
        return -1;
    if( us < 0.0001*Dcj )
    {
        shock.V = Vp0;
        shock.e = ep0;
        shock.P = P0;
        shock.u = u0;
        shock.us = u0;
        return 0;
    }
    else if( Dcj < us )
        us = Dcj;
	double M = us/state0.c;    
	double M2 = M*M;		
    double b = gamma*(M2-1.)/(gamma+1);
    double c = 2.*gamma/(gamma+1.)*delQ*M2;
    double dp = b + sqrt(b*b -c);
    
    double epsilon = dp/(gamma*M2);
	shock.V  = (1. - epsilon)*V0;
	shock.P  = P0*(dp+1.);
	shock.e  = shock.P*shock.V/(gamma-1.);
	shock.u  = u0 + dir*epsilon*us;	
	return 0;
}

int Deflagration_IdealGas::V(double v, int dir, WaveState &shock)
{
	if( v < 0.9999*Vp0 || 1.0001*Vcj < v )
		return -1;
    if( v < 1.0001*Vp0 )
    {
        shock.V = Vp0;
        shock.e = ep0;
        shock.P = P0;
        shock.u = u0;
        shock.us = u0;
        return 0;
    }
    else if( Vcj < v )
        v = Vcj;
    double dV = V0-v;
    double dp = (delQ*V0 +gamma*dV)/(v-0.5*(gamma-1.)*dV);
    
	shock.V  = v;
	shock.P  = (dp+1.)*state0.P;   
	shock.e  = shock.P*shock.V/(gamma-1.);
	double u = dir*sqrt(P0*dp*dV);
	shock.us = u0 - u*V0/dV;
	shock.u  = u0 - u;	
	return 0;
}

int Deflagration_IdealGas::u(double u, int dir, WaveState &shock)
{
    shock.u = u;
    u = dir*(u-u0);
	if( u < 1.0001*ucj || -0.0001*ucj < u )   // note ucj < 0
		return -1;
    if( u < ucj )
        u = ucj;
    else if( 0.0001*ucj < u )
    {
        shock.V = Vp0;
        shock.e = ep0;
        shock.P = P0;
        shock.u = u0;
        shock.us = u0;
        return 0;
    }
    double u2 = u*u/(P0*V0);
    double b = 0.25*(gamma+1.)*u2+0.5*delQ;
    double dp =  b - sqrt(b*b+gamma*u2);
	shock.P  = (dp+1.)*state0.P;   
    
    double dv = u2/dp;
    shock.V = V0*(1-dv);
	shock.e  = shock.P*shock.V/(gamma-1.);
	shock.us = u0 + dir*u/dv;        
	return 0;
}
