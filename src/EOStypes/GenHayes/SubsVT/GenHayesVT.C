#include <EOS_VT.h>
#include "GenHayes_VT.h"

GenHayes_VT::GenHayes_VT(const char *eos_type) : EOS_VT(eos_type)
{
	// Null
}

GenHayes_VT::~GenHayes_VT()
{
	// Null
}

double GenHayes_VT::P(double V, double T)
{ // pressure
    double thetaV = theta(V);
    double sT = T/thetaV;
	return Pc(V) + thetaV*Dln_theta(V)*IntSCv(sT);
}

double GenHayes_VT::e(double V, double T)
{ // specific energy
    double thetaV = theta(V);
    double sT = T/thetaV;
	return ec(V) + thetaV*IntSCv(sT);
}

double GenHayes_VT::S(double V, double T)
{ // specific entropy
    double thetaV = theta(V);
    double sT = T/thetaV;
	return IntSCvT(sT);
}

double GenHayes_VT::P_V(double V, double T)
{ // P_V = (d/dV)P @ const T
    double thetaV = theta(V);
    double sT = T/thetaV;
	return dPc(V) - D2theta(V)*IntSCv(sT) + sqr(Dln_theta(V))*T*SCv(sT);
}

double GenHayes_VT::P_T(double V, double T)
{ // P_T = (d/dT)P @ const V
    double thetaV = theta(V);
    double sT = T/thetaV;
	return Dln_theta(V)*SCv(sT);
}

double GenHayes_VT::CV(double V, double T)
{ // specific heat at constant V
    double thetaV = theta(V);
    double sT = T/thetaV;
	return SCv(sT);
}

double GenHayes_VT::F(double V, double T)
{ // Helmhotz free energy = e - T*S
    double thetaV = theta(V);
    double sT = T/thetaV;
	return ec(V) - T*IntSCvT(sT) + thetaV*IntSCv(sT);
}
//
// 
double GenHayes_VT::Gamma(double V, double)
{ // -(V/T)dT/dV @ const S
	return V*Dln_theta(V);
}
