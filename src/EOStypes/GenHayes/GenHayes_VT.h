#ifndef EOSLIB_GENHAYES_EOS_VT_H
#define EOSLIB_GENHAYES_EOS_VT_H

#include "GenHayesFunc.h"

// abstract base class for generalized Hayes EOS
class GenHayes_VT : public EOS_VT, public virtual GenHayesFunc
{
private:
	void operator=(const GenHayes_VT&);	    // disallowed
	GenHayes_VT();			                // disallowed
	GenHayes_VT(const GenHayes_VT&);	    // disallowed
	GenHayes_VT *operator &();			    // disallowed, use Duplicate()
protected:
	GenHayes_VT(const char *eos_type);    
public:
	virtual ~GenHayes_VT();	
 // EOS functions
    double P(double V, double T);           // Pressure
    double e(double V, double T);           // specific energy
    double S(double V, double T);           // entropy
    double P_V(double V, double T);         // dP/dV @ const T
    double P_T(double V, double T);         // dP/dT @ const V
    double CV(double V, double T);          // T*(d/dT)S
 //
    double F(double V, double T);           // F = e - T*S
    double Gamma(double V, double T);       // -(V/T)dT/dV @ const S
};

#endif // EOSLIB_GENHAYES_EOS_VT_H
