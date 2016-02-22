#ifndef EOSLIB_JWL_VT_H
#define EOSLIB_JWL_VT_H

#include <EOS_VT.h>

#define JWL_VT_vers "JWL_VTv2.0.1"
#define JWL_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *JWL_VTlib_vers;
extern const char *JWL_VTlib_date;

extern "C" { // DataBase functions
    char *EOS_VT_JWL_Init();
    char *EOS_VT_JWL_Info();
    void *EOS_VT_JWL_Constructor();
}
//
//
class EOS;
class JWL;
//
//
class JWL_VT : public EOS_VT
{
private:
    void operator=(const JWL_VT&);              // disallowed
    JWL_VT(const JWL_VT&);                      // disallowed
    JWL_VT *operator &();                       // disallowed, use Duplicate()
protected:
    EOS *Ve();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    JWL_VT();
    ~JWL_VT();
    JWL_VT(JWL &eos);
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
    double P(double V, double T);               // Pressure
    double e(double V, double T);               // specific energy
    double S(double V, double T);               // entropy
    double P_V(double V, double T);             // dP/dV @ const T
    double P_T(double V, double T);             // dP/dT @ const V
    double CV(double V, double T);              // T*(d/dT)S
    //
    double Gamma(double V, double T);           // -(V/T)dT/dV @ const S
    double c2(double V, double T);              // adiabatic (sound speed)^2
    int NotInDomain(double V, double T);
// reference curve is isentrope thru CJ state
    double Pref(double V);
    double eref(double V);
    double Tref(double V);
};

#endif // EOSLIB_JWL_VT_H
