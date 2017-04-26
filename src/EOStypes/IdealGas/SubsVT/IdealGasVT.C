#include "EOS_VT.h"
#include "IdealGas_VT.h"
#include <cmath>

IdealGas_VT::IdealGas_VT() : EOS_VT("IdealGas"), gamma(5./3.), Cv(1.0)
{
	V_ref = 1;
	T_ref = 300;
	EOSstatus = EOSlib::good;
}

IdealGas_VT::IdealGas_VT(double g, double C_v) : EOS_VT("IdealGas"),
					        gamma(g), Cv(C_v)
{
	V_ref = 1;
	T_ref = 300;
	EOSstatus = EOSlib::good;
}


IdealGas_VT::~IdealGas_VT()
{
	// Null
}
//
//
//
double IdealGas_VT::P(double V, double T)
{ // pressure
	return (gamma-1)*Cv*T/V;
}

double IdealGas_VT::e(double, double T)
{ // specific energy
	return Cv*T;
}

double IdealGas_VT::S(double V, double T)
{ // specific entropy
	return Cv*(log(T/T_ref) + (gamma-1)*log(V/V_ref));
}

double IdealGas_VT::P_V(double V, double T)
{ // P_V = (d/dV)P @ const T
	return -(gamma-1)*Cv*T/sqr(V);
}

double IdealGas_VT::P_T(double V, double)
{ // P_T = (d/dT)P @ const V
	return (gamma-1)*Cv/V;
}

double IdealGas_VT::CV(double, double)
{ // specific heat at constant V
	return Cv;
}

double IdealGas_VT::F(double V, double T)
{ // Helmhotz free energy = e - T*S
	return Cv*T*(1 - log(T/T_ref) - (gamma-1)*log(V/V_ref));
}

int IdealGas_VT::NotInDomain(double V, double T)
{
	return (V<=0 || T<0) ? -1 : 0;
}
//
//
double IdealGas_VT::CP(double, double)
{ // Specific heat at constant P
    return gamma*Cv;
}

double IdealGas_VT::KS(double V, double T)
{ // Isentropic bulk modulus
    return gamma*P(V,T);
}

double IdealGas_VT::KT(double V, double T)
{ // Isothermal bulk modulus
    return P(V,T);
}
    
double IdealGas_VT::Gamma(double, double)
{ // -(V/T)dT/dV @ const S
	return gamma - 1;
}

double IdealGas_VT::beta(double, double T)
{ // volumetric thermal expansion
    return 1/T;
}

double IdealGas_VT::c2(double V, double T)
{ // adiabatic (sound speed)^2
	return gamma*(gamma-1)*Cv*T;    
}

double IdealGas_VT::cT2(double, double T)
{ // isothermal, c2 = -V^2(d/dV)P
	return (gamma-1)*Cv*T;
}


