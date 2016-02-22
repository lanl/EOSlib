#include <EOS.h>
#include "HayesBM.h"

double HayesBM::P(double V, double e)
{
	if( V <= 0. )
		return NaN;
	return P(V) + Gamma(V)/V *(e-eT0(V));
}

double HayesBM::e_PV(double p, double V)
{
	if( V <= 0. )
		return NaN;
	return eT0(V) + V*(p-P(V))/Gamma(V);
}

double HayesBM::T(double V, double e)
{
	if( V <= 0. )
		return NaN;
	return T0 + (e-eT0(V))/Cv;
}


double HayesBM::S(double V, double e)
{
	if( V <= 0. )
		return NaN;
	return Cv*(log(1+(e-eT0(V))/(Cv*T0)) - Gp(V));
}



double HayesBM::c2(double V, double e)
{
	if( V <= 0. )
		return NaN;
    double G = Gamma(V);
    double dT = (e-eT0(V))/Cv;
    return -V*V*(dP(V)+Cv*dT*dG(V)) + G*G*Cv*(T0+dT);
}

double HayesBM::Gamma(double V, double)
{
	return V>0. ? Gamma(V) : NaN;
}

double HayesBM::CV(double V, double)
{
	return Cv;
}

double HayesBM::FD(double V, double e)
{
	if( V <= 0. )
		return NaN;

    double G = Gamma(V)/V;
    double dT = (e-eT0(V))/Cv;
    return 0.5*(V*V*V)*(d2P(V)+Cv*dT*d2G(V)+Cv*(T0+dT)*G*(G*G-3.*dG(V)))/c2(V,e);
}

/************
int HayesBM::PT(double P, double T, HydroState &state)
{
	if( P<0 || T<0 )
		return 1;
	state.V = V_PT(P,T);
	if( isnan(state.V) || state.V <= 0 )
		return 1;
		
	state.e = e_VT(state.V,T);
	return 0;
}
***************/

int HayesBM::NotInDomain(double V, double e)
{
	return V <= 0. || P(V,e) < 0. || T(V,e) < 0.;
}
