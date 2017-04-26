#include "IdealHE.h"
#include "IdealHE_Hugoniot.h"

Detonation_IdealHE::Detonation_IdealHE(IdealHE &gas, double pvac)
                        : Detonation(gas,pvac)
{
    gamma   = gas.gamma;
    lambda0 = gas.lambda_ref();
    Q  = gas.Q;
    Cv = gas.Cv;
}

Detonation_IdealHE::~Detonation_IdealHE()
{
	// Null
}

int Detonation_IdealHE::InitCJ()
{
    double e = e0+lambda0*Q;
    if( e <= 0. || gamma <= 1. || Q <= 0. )
        return 1;
    //P0 = (gamma-1.)*(e0+lambda_eff*Q)/V0;
    state0.c = sqrt(gamma*(gamma-1.)*e);
    state0.T = e/Cv;
    delQ = e/(P0*V0) -1./(gamma-1.);


    double b = 1.+(gamma-1.)*delQ;
    double c = 1.+2.*(gamma-1.)/(gamma+1.)*delQ;
    Pcj = (b + sqrt(b*b-c))*P0;
    double dP = Pcj-P0;
    Vcj = gamma*Pcj/(gamma*Pcj+dP)*V0;
    ecj = Pcj*Vcj/(gamma-1.) - Q;
    Dcj = sqrt(gamma*Pcj/Vcj)*V0;
    ucj = (1-Vcj/V0)*Dcj;
    return 0;    
}

int Detonation_IdealHE::P(double p, int direction, WaveState &shock)
{
	if( p < 0.9999*Pcj )
		return -1;
    if( p < Pcj )
        p = Pcj;
    double dp = (p-P0)/P0;
    double epsilon = (dp -(gamma-1.)*delQ)/(gamma+0.5*(gamma+1.)*dp);
	shock.V  = (1.-epsilon)*V0;
	shock.P  = p;
	shock.e  = p*shock.V/(gamma-1.)-Q;
	double us = direction*sqrt(dp*P0*V0/epsilon);
    shock.u  = u0 + epsilon*us;
	shock.us = u0 + us;
	return 0;
}

int Detonation_IdealHE::u_s(double us, int direction, WaveState &shock)
{
	shock.us = us;
    us = direction*(us-u0);
	if( us < 0.9999*Dcj )
		return -1;
    if( us < Dcj )
        us = Dcj;
	double M = us/sqrt(gamma*P0*V0);    
	double M2 = M*M;		
    double b = gamma*(M2-1.)/(gamma+1);
    double c = 2.*gamma*(gamma-1.)/(gamma+1.)*delQ*M2;
    double dp = b + sqrt(b*b -c);
    
    double epsilon = dp/(gamma*M2);
	shock.V  = (1. - epsilon)*V0;
	shock.P  = P0*(dp+1.);
	shock.e  = shock.P*shock.V/(gamma-1.)-Q;
	shock.u  = u0 + epsilon*us;	
	return 0;
}
int Detonation_IdealHE::w_u_s(double us, int direction, WaveState &shock)
{
    return 1; // To do
}

int Detonation_IdealHE::V(double v, int direction, WaveState &shock)
{
    if( 1.0001*Vcj < v )
        return -1;
    if( Vcj < v )
        v = Vcj;
    else if( v/V0 <= (gamma-1.)/(gamma+1.) )
		return -1;
    double dV = V0-v;
    double dp = ((gamma-1.)*delQ*V0 +gamma*dV)/(v-0.5*(gamma-1.)*dV);
    
	shock.V  = v;
	shock.P  = (dp+1.)*P0;   
	shock.e  = shock.P*shock.V/(gamma-1.)-Q;
	double u = direction*sqrt(P0*dp*dV);
	shock.us = u0 + u*V0/dV;
	shock.u  = u0 + u;	
	return 0;
}

int Detonation_IdealHE::u(double u, int direction, WaveState &shock)
{
    shock.u = u;
    u = direction*(u-u0);
	if( u < 0.9999*ucj )
		return -1;
    if( u < ucj )
        u = ucj;
    double u2 = u*u/(P0*V0);
    double b = 0.25*(gamma+1.)*u2+0.5*(gamma-1.)*delQ;
    double dp =  b +sqrt(b*b+gamma*u2);
	shock.P  = (dp+1.)*P0;   
    double dv = u2/dp;
    shock.V = V0*(1-dv);
	shock.e  = shock.P*shock.V/(gamma-1.)-Q;
	shock.us = u0 + u/dv;        
	return 0;
}
