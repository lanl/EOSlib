#include "EOS_VT.h"
#include "Hayes_VT.h"

double Hayes_VT::P(double V, double T)
{ // pressure
	return P0 + (Gamma0/V0)*Cv*(T-T0)
           + (K0/N)*(pow(V0/V,N)-1);
}

double Hayes_VT::e(double V, double T)
{ // specific energy
    double v = V/V0;
	return e0+P0*(V0-V)+Cv*(T-T0*(1+Gamma0*(1-v)))
           + K0*V0/N*((pow(v,1-N)-1)/(N-1)-(1-v));
}

double Hayes_VT::S(double V, double T)
{ // specific entropy
	return Cv*(log(T/T0) - Gamma0*(V0-V)/V0);
}

double Hayes_VT::P_V(double V, double T)
{ // P_V = (d/dV)P @ const T
	return -(K0/V)*pow(V0/V,N);
}

double Hayes_VT::P_T(double V, double)
{ // P_T = (d/dT)P @ const V
	return (Gamma0/V0)*Cv;
}

double Hayes_VT::CV(double, double)
{ // specific heat at constant V
	return Cv;
}

double Hayes_VT::F(double V, double T)
{ // Helmhotz free energy = e - T*S
    double v = V/V0;
	return e0 +P0*(V0-V) 
           + Cv*(T-T0)*(1+Gamma0*(1-v))- Cv*T*log(T/T0)
           + K0*V0/N*((pow(v,1-N)-1)/(N-1)-(1-v));
}
//
//
double Hayes_VT::CP(double V, double T)
{ // Specific heat at constant P
    return (1+sqr(Gamma0/V0)*V*Cv*T/K0*pow(V/V0,N))*Cv;
}

double Hayes_VT::KS(double V, double T)
{ // Isentropic bulk modulus
    return K0*pow(V0/V,N) + sqr(Gamma0/V0)*V*Cv*T;
}

double Hayes_VT::KT(double V, double T)
{ // Isothermal bulk modulus
	return K0*pow(V0/V,N);
}
    
double Hayes_VT::Gamma(double V, double)
{ // -(V/T)dT/dV @ const S
	return Gamma0*V/V0;
}

double Hayes_VT::beta(double V, double)
{ // volumetric thermal expansion
	return Gamma0*Cv/(V0*K0)*pow(V/V0,N);
}

double Hayes_VT::c2(double V, double T)
{ // adiabatic (sound speed)^2
	return K0*V*pow(V0/V,N) + sqr(Gamma0*V/V0)*Cv*T;
}

double Hayes_VT::cT2(double V, double)
{ // isothermal, c2 = -V^2(d/dV)P
	return K0*V*pow(V0/V,N);
}
