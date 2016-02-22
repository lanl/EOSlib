#ifndef EOSLIB_GENHAYES_Ve_EOS_H
#define EOSLIB_GENHAYES_Ve_EOS_H

#include "GenHayesFunc.h"
//
// abstract base class for generalized Hayes EOS
// Free energy, F(V,T) = ec(V) - T*IntSCvT(sT) + theta(V)*IntSCv(sT)
// EOS functions built out of low level functions
//     ec(V), theta(V), IntSCv(sT), IntSCvT(sT), etc
// from interface class GenHayesFunc
// Example of usage
//   class BirchMurnaghan    : public GenHayes,    public BirchMurnaghanBase
//   class BirchMurnaghan_VT : public GenHayes_VT, public BirchMurnaghanBase
//   class BirchMurnaghanBase: public BirchMurnaghanParams, public IntSCv_ODE,
//                             public virtual GenHayesFunc
//   virtual GenHayesFunc enables low level functions in BirchMurnaghanBase
//           and associated parameters BirchMurnaghanParams
//           to be used by both BirchMurnaghan and BirchMurnaghan_VT
//
class GenHayes : public EOS, public virtual GenHayesFunc
{
private:
	void operator=(const GenHayes&);	    // disallowed
	GenHayes();			                    // disallowed
	GenHayes(const GenHayes&);			    // disallowed
	GenHayes *operator &();			        // disallowed, use Duplicate()
protected:
	GenHayes(const char *eos_type);    
public:
	virtual ~GenHayes();
 // EOS functions
	double P(double V, double e); 	        // Pressure
	double T(double V, double e);   	    // Temperature
	double S(double V, double e);	        // Entropy	
	double c2(double V, double e);		    // sound speed squared
	double Gamma(double V, double e);	    // Gruneisen coefficient
	double CV(double V, double e);		    // Specific heat
    double e_PV(double p, double V);
    
	int NotInDomain(double V, double e);
};

#endif // EOSLIB_GENHAYES_Ve_EOS_H
