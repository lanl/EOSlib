#include "EOS_VT.h"
#include "HayesBM_VT.h"

double HayesBM_VT::P(double V, double T)
{ // pressure
	return P(V) + Cv*(T-T0)*Gamma(V)/V;
}

double HayesBM_VT::e(double V, double T)
{ // specific energy
	return e(V) + Cv*(T-T0)-Cv*T0*Gp(V);
}

double HayesBM_VT::S(double V, double T)
{ // specific entropy
	return Cv*(log(T/T0) - Gp(V));
}

double HayesBM_VT::P_V(double V, double T)
{ // P_V = (d/dV)P @ const T
	return dP(V) + Cv*(T-T0)*dG(V);
}

double HayesBM_VT::P_T(double V, double)
{ // P_T = (d/dT)P @ const V
	return Cv*Gamma(V)/V;
}

double HayesBM_VT::CV(double, double)
{ // specific heat at constant V
	return Cv;
}

double HayesBM_VT::F(double V, double T)
{ // Helmhotz free energy = e - T*S
	return e(V) +Cv*(T-T0)*(1.+Gp(V)) - Cv*T*log(T/T0);
}
//
//
double HayesBM_VT::CP(double V, double T)
{ // Specific heat at constant P
    double G = Gamma(V);
	double c2t = -V*V*(dP(V)+Cv*(T-T0)*dG(V));
    return Cv*(1.+ G*G*Cv*T/c2t);
}

double HayesBM_VT::KS(double V, double T)
{ // Isentropic bulk modulus
    double G = Gamma(V);
	return -V*(dP(V)+Cv*(T-T0)*dG(V))+G*G*Cv*T/V;
}

double HayesBM_VT::KT(double V, double T)
{ // Isothermal bulk modulus
    double G = Gamma(V);
	return -V*(dP(V)+Cv*(T-T0)*dG(V));
}
    
double HayesBM_VT::Gamma(double V, double)
{ // -(V/T)dT/dV @ const S
	return Gamma(V);
}

double HayesBM_VT::beta(double V, double T)
{ // volumetric thermal expansion
	double c2t = -V*V*(dP(V)+Cv*(T-T0)*dG(V));
	return Cv*Gamma(V)/c2t;
}

double HayesBM_VT::c2(double V, double T)
{ // adiabatic (sound speed)^2
    double G = Gamma(V);
	return -V*V*(dP(V)+Cv*(T-T0)*dG(V)) +G*G*Cv*T;
}

double HayesBM_VT::cT2(double V, double T)
{ // isothermal, c2 = -V^2(d/dV)P
	return -V*V*(dP(V)+Cv*(T-T0)*dG(V));
}
