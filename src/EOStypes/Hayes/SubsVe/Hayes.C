#include <EOS.h>
#include "Hayes.h"

Hayes::Hayes() : EOS("Hayes")
{
	// Null
}

Hayes::~Hayes()
{
	// Null
}

double Hayes::P(double V, double e)
{
	if( V <= 0. )
		return NaN;
	double v = V0/V;
	double dV = V0-V;
	double GammaV = Gamma0/V0;
	double p;
	p = P0 + (K0/N)*( pow(v,N)-1. -Gamma0/(N-1)*(pow(v,N-1.)-1.) + GammaV*dV )
		  + GammaV*(e-e0+Cv*T0*GammaV*dV - P0*dV);
	return p;
}
double Hayes::e_PV(double p, double V)
{
	if( V <= 0. )
		return NaN;
	double v = V0/V;
	double dV = V0-V;
	double GammaV = Gamma0/V0;
    double pref = P0 + (K0/N)*( pow(v,N)-1. -Gamma0/(N-1.)*(pow(v,N-1.)-1.)
                                + GammaV*dV );
    return  e0 - Cv*T0*GammaV*dV + P0*dV + V0*(p-pref)/Gamma0;
}

double Hayes::T(double V, double e)
{
	if( V <= 0. )
		return NaN;
	double dV = V0 - V;
	return T0 + T0*(Gamma0/V0)*dV +
	       ( e-e0-P0*dV -(K0*V0/N)*( (pow(V0/V,N-1)-1)/(N-1)-dV/V0 ) )/Cv;
}


double Hayes::S(double V, double e)
{
	return V > 0. ? Cv*( log(T(V,e)/T0) - (Gamma0/V0)*(V0-V) ) : NaN;
}



double Hayes::c2(double V, double e)
{
	if( V <= 0. )
		return NaN;
	double v_N = pow(V0/V,N);
	double GammaV = Gamma0/V0;
	return K0*V*v_N + GammaV*(P(V,e)-P0-(K0/N)*(v_N-1)+Cv*T0*GammaV)*V*V;	
}

double Hayes::Gamma(double V, double)
{
	return V>0. ? Gamma0*(V/V0) : NaN;
}

double Hayes::CV(double V, double)
{
	return Cv;
}

double Hayes::FD(double V, double e)
{
	if( V <= 0. )
		return NaN;
	double G = Gamma(V,e);
	double R = K0*pow(V0/V,N)*V/(G*Cv*T(V,e));
	return 0.5 * ((N+1.)*R  + G*G)/(R+G);
}

int Hayes::PT(double P, double T, HydroState &state)
{
	if( P<0. || T<0. )
		return 1;
	state.V = V_PT(P,T);
	if( std::isnan(state.V) || state.V <= 0. )
		return 1;
		
	state.e = e_VT(state.V,T);
	return 0;
}


int Hayes::NotInDomain(double V, double e)
{
	return V <= 0. || P(V,e) < 0. || T(V,e) < 0.;
}
