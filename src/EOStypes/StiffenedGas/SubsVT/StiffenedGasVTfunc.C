#include "EOS_VT.h"
#include "StiffenedGas_VT.h"

double StiffenedGas_VT::P(double V, double T)
{ // pressure
	return G*Cv*T/V - P_p;
}

double StiffenedGas_VT::e(double V, double T)
{ // specific energy
	return Cv*T + P_p*V + e_p;
}

double StiffenedGas_VT::S(double V, double T)
{ // specific entropy
	return Cv*(log(T/T_ref) + G*log(V/V_ref));
}

double StiffenedGas_VT::P_V(double V, double T)
{ // P_V = (d/dV)P @ const T
	return -G*Cv*T/V/V;
}

double StiffenedGas_VT::P_T(double V, double)
{ // P_T = (d/dT)P @ const V
	return G*Cv/V;
}

double StiffenedGas_VT::CV(double, double)
{ // specific heat at constant V
	return Cv;
}

double StiffenedGas_VT::F(double V, double T)
{ // Helmhotz free energy = e - T*S
	return Cv*T*(1 - log(T/T_ref) - G*log(V/V_ref)) + P_p*V + e_p;
}

int StiffenedGas_VT::NotInDomain(double V, double T)
{
	return (V<=0 || T<0 && G*Cv*T/V > P_p) ? -1 : 0;
}
//
//
double StiffenedGas_VT::CP(double, double)
{ // Specific heat at constant P
    return (G+1)*Cv;
}

double StiffenedGas_VT::KS(double V, double T)
{ // Isentropic bulk modulus
    return (G+1)*G*Cv*T/V;
}

double StiffenedGas_VT::KT(double V, double T)
{ // Isothermal bulk modulus
	return G*Cv*T/V;
}
    
double StiffenedGas_VT::Gamma(double, double)
{ // -(V/T)dT/dV @ const S
	return G;
}

double StiffenedGas_VT::beta(double, double T)
{ // volumetric thermal expansion
	return 1/T;
}

double StiffenedGas_VT::c2(double, double T)
{ // adiabatic (sound speed)^2
	return (G+1)*G*Cv*T;
}

double StiffenedGas_VT::cT2(double, double T)
{ // isothermal, c2 = -V^2(d/dV)P
	return G*Cv*T;
}
