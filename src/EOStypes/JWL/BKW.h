#ifndef EOSLIB_BKW_EOS_H
#define EOSLIB_BKW_EOS_H

#include <EOS.h>

#define BKW_vers "BKWv2.0.1"
#define BKW_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *BKWlib_vers;
extern const char *BKWlib_date;

extern "C" { // DataBase functions
    char *EOS_BKW_Init();
    char *EOS_BKW_Info();
    void *EOS_BKW_Constructor();
}


class BKWdetonation;
class BKWdeflagration;
class BKW;
class EOS_VT;
class BKW_VT;
//
// HOM gas EOS is Mie-Gruneisen fit to
// Becker-Kistiakowsky-Wilson EOS
// using isentrope thru CJ state as reference curve & Cv = constant
//   P(V,e) = Pref(V) + [Gamma(V)/V][e + de - eref(V)]
//   T(V,e) = Tref(V) + [e + de - eref(V)]/Cv
//   S(V,e) = Cv*ln[T(V,e)/Tref(V)] + Scj
//   where
//     ln(Pref) = sum(n=0,4) Pn*[ln(V)]^n
//     ln(eref) = sum(n=0,4) en*[ln(Pref)]^n
//       de is offset of energy origin between reactants and products EOS
//       note, consistency requires eref(V) = -int dV Pref(V)
//     ln(Tref) = sum(n=0,4) Tn*[ln(V)]^n
//     Gamma(V) = - dln(Tref)/dln(V)
//              = -sum(n=1,4) n*Tn*[ln(V)]^(n-1)
// ---------------------
// Ref: Numerical Modeling of Explosives & Propellants
//      C. L. Mader
//      Appendix A.9 HOM for Gas Components
//
class BKW : public EOS
{
private:
	void operator=(const BKW&);		// disallowed
	BKW(const BKW&);			    // disallowed
	BKW *operator &();			    // disallowed, use Duplicate()
protected:
    EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
public:
	BKW();
	BKW(BKW_VT &eos);
	~BKW();
// parameters
	double V0, e0, P0;	            // reference state for HE
    double de;                      // offset in energy origin
    double Pn[5];                   // coef for fit to Pref
    double en[5];                   // coef for fit to eref
    double Tn[5];                   // coef for fit to Tref
    double Cv;                      // specific heat
    double Scj;                     // entropy at CJ state
    // units for polynomial fits
    // ln(Pref/Punit) = function ln(V/Vunit) etc.
    double Punit;                   // unit for pressure
    double eunit;                   // unit for energy
    double Vunit;                   // unit for specific volume
    double Tunit;                   // unit for temperature
// derived parameters
    double Vcj;
    double Pcj;
    double Tcj;
// EOS functions
	double P(double V, double e); 	    // Pressure
	double c2(double V, double e);		// sound speed squared
	double Gamma(double V, double e);	// Gruneisen coefficient
	double FD(double V, double e);		// Fundamental Derivative
	int NotInDomain(double V, double e);
    double T(double V, double e);       // Temperature
	double S(double V, double e);	    // Entropy	
	double CV(double V, double e);		// Specific heat
	double e_PV(double p, double V);
// reference curve is isentrope thru CJ state
    double Pref(double V);
    double eref(double V);
    double Tref(double V);
    double c2ref(double V);             // -V^2 *(d/dV)Pref  = -V*(d/dlnV)Pref
    double dc2ref(double V);            //  V   *(d/dV)c2ref =  (d/dlnV)c2ref
    //
    double lnPref(double lnV);
    double gref(double lnV);            // gamma_ref = -dlnPref/dlnV
    double lneref(double lnP);
    double lnTref(double lnV);
    double Gref(double lnV);            // Gamma_ref = - dlnTref/dlnV
// detonation allows non-equilibirum initial state for Hugoniot locus
    Detonation *detonation(const HydroState &state, double P0);
    Deflagration *deflagration(const HydroState &state, double P0);
};

inline double BKW::lnPref(double lnV)
    { return (((Pn[4]*lnV+Pn[3])*lnV+Pn[2])*lnV+Pn[1])*lnV+Pn[0]; }
inline double BKW::gref(double lnV)
    { return -((4.*Pn[4]*lnV+3.*Pn[3])*lnV+2.*Pn[2])*lnV-Pn[1]; }
inline double BKW::lneref(double lnP)
    { return (((en[4]*lnP+en[3])*lnP+en[2])*lnP+en[1])*lnP+en[0]; }
inline double BKW::lnTref(double lnV)
    { return (((Tn[4]*lnV+Tn[3])*lnV+Tn[2])*lnV+Tn[1])*lnV+Tn[0]; }
inline double BKW::Gref(double lnV)
    { return -((4.*Tn[4]*lnV+3.*Tn[3])*lnV+2.*Tn[2])*lnV-Tn[1]; }

#endif // EOSLIB_BKW_EOS_H
