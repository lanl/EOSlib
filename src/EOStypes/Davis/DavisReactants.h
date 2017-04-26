#ifndef EOSLIB_DAVIS_REACTANTS_EOS_H
#define EOSLIB_DAVIS_REACTANTS_EOS_H

#include <EOS.h>

#define DavisReactants_vers "DavisEOSv2.0.1"
#define DavisReactants_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *DavisReactantslib_vers;
extern const char *DavisReactantslib_date;

extern "C" { // DataBase functions
    char *EOS_DavisReactants_Init();
    char *EOS_DavisReactants_Info();
    void *EOS_DavisReactants_Constructor();
}


class DavisReactants : public EOS
{
private:
	void operator=(const DavisReactants&);		// disallowed
	DavisReactants(const DavisReactants&);		// disallowed
	DavisReactants *operator &();			    // disallowed, use Duplicate()
protected:
    //EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
public:
	DavisReactants();
	//DavisReactants(DavisReactants_VT &eos);
	~DavisReactants();
// parameters
    double V0;
    double e0;
    double P0;
    double T0;
    double S0;
    //
    double A;
    double B;
	double C;
    double G0;
    double Z;       // Gamma = G0 + Z*(1-V/V0)
    double Cv;
    double alpha;   // CV = Cv0 + alpha*(S-S0)
    // derived
    double ps;      // ps = 0.25*rho0*A^2/B
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
// detonation allows non-equilibirum initial state for Hugoniot locus
    //Detonation *detonation(const HydroState &state, double P0);
    //Deflagration *deflagration(const HydroState &state, double P0);
};

#endif // EOSLIB_DAVIS_REACTANTS_EOS_H
