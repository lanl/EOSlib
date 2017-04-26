#ifndef EOSLIB_HEprods_EOS_H
#define EOSLIB_HEprods_EOS_H

#include <EOS.h>
#include <OneDFunction.h>

#define HEprods_vers "HEfitv2.0.0"
#define HEprods_date "August 15, 2015"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *HEprodslib_vers;
extern const char *HEprodslib_date;

extern "C" { // DataBase functions
    char *EOS_HEprods_Init();
    char *EOS_HEprods_Info();
    void *EOS_HEprods_Constructor();
}


class HEprodsDetonation;
class HEprods;

// Sam Shaw's fitting form for experimental data of detonation products
// Mie-Gruneisen form based on CJ release isentrope as reference curve
// ---------------------
// for 9502 parameters
// The SURF model and the curvature effect for PBX 9502
//      Combustion Theory and Modelling 16 (2012) pp 1140-1169
//      Ralph Menikoff and Sam Shaw
//      http://dx.doi.org/10.1080/13647830.2012.713994
//      Appendix B.2
// for 9501 parameters
// Deflagration Wave Profiles LA-UR-20353-rev
//      Appendix D.2 for fitting form for CJ isentrope
//      Pcj(rho) = rho^gamma1 sum a1(n) rho^n   for rho < rho_sw
//               = rho^gamma2 sum a2(n) rho^n   for rho > rho_sw
//      rho Gamma(rho) = sum b1(n) rho^n   for rho < rho_sw
//                     = sum b2(n) rho^n   for rho > rho_sw

class HEprods : public EOS
{
    friend class HEprodsDetonation;
private:
	int F(double *y_prime, const double *y, double rho);  // ODE::F
    
	void operator=(const HEprods&);         // disallowed
	HEprods(const HEprods&);			    // disallowed
	HEprods *operator &();                  // disallowed, use Duplicate()
protected:
// ambient reactant state
	double V0;
    double e0;
    double P0;
// CJ state relative to ambient reactant state
    double rho_cj;      // derived
    double   e_cj;      // derived
    double   P_cj;      // derived
    double   T_cj;
    double   S_cj;
// bounds on domain
    double rho_min, rho_max;
// Fitting form parameters
    double rho_sw;
    double e_sw;        // derived
    double T_sw;        // derived
    // Pcj
    double gamma1, gamma2;
    int na1, na2;
    double a1[12], a2[12];
    // Gamma
    int nb1, nb2;
    double b1[12], b2[12];
    // thermal
    double Cv;                              // specific heat

    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
	HEprods();
	~HEprods();

// EOS functions
	double P(double V, double e); 	        // Pressure
	double c2(double V, double e);		    // sound speed squared
	double Gamma(double V, double e);	    // Gruneisen coefficient
	//double FD(double V, double e);		    // Fundamental Derivative
	int NotInDomain(double V, double e);
    double T(double V, double e);           // Temperature
	double S(double V, double e);	        // Entropy	
	double CV(double V, double e);		    // Specific heat
	double e_PV(double p, double V);
// reference curve is isentrope thru CJ state
    double Pref(double V);
    double eref(double V);
    double c2ref(double V);
    double Tref(double V);
    double  GammaVref(double V);            // Gamma/V = rho*Gamma
    double dGammaVref(double V);            // d(rho*Gamma)/drho
// functions to calculate sonic state on CJ isentrope
    double fsonic(double rho);
    double FindCJ();
// detonation allows non-equilibirum initial state for Hugoniot locus
    //Detonation *detonation(const HydroState &state, double P0);
    //Deflagration *deflagration(const HydroState &state, double P0);
};

#endif // EOSLIB_HEprods_EOS_H

