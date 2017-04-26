#ifndef EOSLIB_JWL_EOS_H
#define EOSLIB_JWL_EOS_H

#include <EOS.h>
#include <OneDFunction.h>

#define JWL_vers "JWLv2.0.1"
#define JWL_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

using namespace std;

extern const char *JWLlib_vers;
extern const char *JWLlib_date;

extern "C" { // DataBase functions
    char *EOS_JWL_Init();
    char *EOS_JWL_Info();
    void *EOS_JWL_Constructor();
}


class JWLdetonation;
class JWLdeflagration;
class JWL;
class EOS_VT;
class JWL_VT;
//
// Jones-Wilkins-Lee EOS used for detonation products
//   P(V,e) =   A*[1-(omega/V)*(V0/R1)]*exp(-R1*V/V0)
//            + B*[1-(omega/V)*(V0/R2)]*exp(-R2*V/V0)
//            + omega*(e + de)/V
//   de = Edet - e0 = offset for origin of e
//   e0 = energy of solid reactant at V0 and P0
// Isentrope of form
//   P_s(V) = A*exp(-R1*V/V0) + B*exp(-R2*V/V0) + C*(V0/V)^(omega+1)
//   e_s(V) =   (A*V0/R1)*exp(-R1*V/V0) 
//            + (B*V0/R2)*exp(-R2*V/V0) 
//            + (C*V0/omega)*(V0/V)^omega
//            - de
//   where A, B are constants and C characterizes the isentrope.
//   For isentrope thru (V1,e1)
//   C=omega*(V1/V0)^omega*[ (e1+de)/V0 - (A/R1)*exp(-R1*V1/V0)
//                                 - (B/R2)*exp(-R2*V1/V0) ]
// Mie-Gruneisen form defines same EOS for any isentrope
//   P(V,e) = P_s(V) + (omega/V)*[e-e_s(V)]
// CJ state is sonic point on detonation locus based on (V0, e0) and P0=0
//   P(V0, e0) = constant volume burn pressure
// Temperature determined by Tcj and constant Cv
// ---------------------
// Ref: Detonation EOS at LLNL, 1993
//      P. C. Souers and L. C. Haselman Jr.
//      Chapter 2. EOS description
//
class JWL : public EOS
{
private:
	void operator=(const JWL&);		// disallowed
	JWL(const JWL&);			    // disallowed
	JWL *operator &();			    // disallowed, use Duplicate()
protected:
    EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
public:
	JWL();
	JWL(JWL_VT &eos);
	~JWL();
// parameters
	double V0, e0;	// reference state
    double A, R1;
    double B, R2;
    double C, omega;                // omega is Gruneisen coefficient
    double Edet;                    // "infinite volume energy of detonation"
    double Cv;                      // specific heat
    double Tcj;                     // temperature at CJ state
    double Scj;                     // entropy at CJ state
// derived parameters
    double de;                      // Edet-e0
    double Vcj;
    double Pcj;
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
// detonation allows non-equilibirum initial state for Hugoniot locus
    Detonation *detonation(const HydroState &state, double P0);
    Deflagration *deflagration(const HydroState &state, double P0);
};

class JWLdetonation : public Detonation, private OneDFunction
{
    enum Zero
    {
        pressure=0, velocity=1, wave_speed=2, CJ=3
    };
private:    
    int InitCJ();
    
    double omega;                               // Gruneisen coefficient
    double Vlimit;                              // limiting V on shock locus
    double f(double);                           // OneDFunction::f
    Zero fzero;                                 // flag for OneDFunction::f
    double Pav, m2, du2;                        // used in OneDFunction::f
    double P1, V1, e1, D;                       // set by OneDFunction::f
    
    JWLdetonation(const JWLdetonation&);        // disallowed
    void operator=(const JWLdetonation&);       // disallowed
    JWLdetonation *operator &();                // disallowed
public:
    double rel_tol;                             // tolerance for near CJ state
    JWLdetonation(JWL &gas, double pvac = EOS::NaN);
    ~JWLdetonation();    

    int P  (double p,  int direction, WaveState &wave);
    int u  (double u,  int direction, WaveState &wave);
    int u_s(double us, int direction, WaveState &wave);
    int w_u_s(double us, int direction, WaveState &wave);
    int V  (double V,  int direction, WaveState &wave);
};

class JWLdeflagration : public Deflagration, private OneDFunction
{
    enum Zero
    {
        pressure=0, velocity=1, wave_speed=2, CJ=3, VMIN=4
    };
private:    
    int InitCJ();
    
    double omega;                               // Gruneisen coefficient
    double f(double);                           // OneDFunction::f
    Zero fzero;                                 // flag for OneDFunction::f
    double Pav, m2, du2;                        // used in OneDFunction::f
    double P1, V1, e1, D;                       // set by OneDFunction::f
    
    JWLdeflagration(const JWLdeflagration&);    // disallowed
    void operator=(const JWLdeflagration&);     // disallowed
    JWLdeflagration *operator &();              // disallowed
public:
    double rel_tol;                             // tolerance for near CJ state
    JWLdeflagration(JWL &gas, double pvac = EOS::NaN);
    ~JWLdeflagration();    

    int P  (double p,  int direction, WaveState &wave);
    int u  (double u,  int direction, WaveState &wave);
    int u_s(double us, int direction, WaveState &wave);
    int V  (double V,  int direction, WaveState &wave);
};

#endif // EOSLIB_JWL_EOS_H
