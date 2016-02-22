#ifndef EOSLIB_DAVIS_PRODUCTS_EOS_H
#define EOSLIB_DAVIS_PRODUCTS_EOS_H

#include <EOS.h>

#define DavisProducts_vers "DavisEOSv2.0.1"
#define DavisProducts_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *DavisProductslib_vers;
extern const char *DavisProductslib_date;

extern "C" { // DataBase functions
    char *EOS_DavisProducts_Init();
    char *EOS_DavisProducts_Info();
    void *EOS_DavisProducts_Constructor();
}


class DavisProducts : public EOS
{
private:
	void operator=(const DavisProducts&);		// disallowed
	DavisProducts(const DavisProducts&);		// disallowed
	DavisProducts *operator &();			    // disallowed, use Duplicate()
protected:
    //EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
public:
	DavisProducts();
	//DavisProducts(DavisProducts_VT &eos);
	~DavisProducts();
// parameters
    double V0;
    double e0;
    double S0;
    //
    double a;
    double n;
	double Vc;      // crossover volume for Gamma
    double pc;      // pc = Pref(Vc)
    double b;
    double k;
    double Cv;      // specific heat
    double Tc;      // Tc = Tref(Vc) [= pow(2,-a*b/n)/(k-1.+a) * pc*Vc/Cv]
    // derived
    double ec;      // ec = eref(Vc)+e0 = pc*Vc/(k-1.+a)
    double T0;      // Tref(V0)
// EOS functions
	double P(double V, double e); 	    // Pressure
	double c2(double V, double e);		// sound speed squared
	double Gamma(double V, double e);	// Gruneisen coefficient
	//double FD(double V, double e);		// Fundamental Derivative
	int NotInDomain(double V, double e);
    double T(double V, double e);       // Temperature
	double S(double V, double e);	    // Entropy	
	double CV(double V, double e);		// Specific heat
    double e_PV(double p, double V);
// reference curve is isentrope thru CJ state
    double Pref(double V);
    double dPref(double V);     // (d/dV)Pref
    double eref(double V);
    double Tref(double V);
    double fref(double vn) { return 2.*a/(1.+vn*vn);}
// detonation allows non-equilibirum initial state for Hugoniot locus
    //Detonation *detonation(const HydroState &state, double P0);
    //Deflagration *deflagration(const HydroState &state, double P0);
};

#endif // EOSLIB_DAVIS_PRODUCTS_EOS_H
