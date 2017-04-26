#ifndef EOSLIB_BKW_VT_H
#define EOSLIB_BKW_VT_H

#include <EOS_VT.h>

#define BKW_VT_vers "BKW_VTv2.0.1"
#define BKW_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *BKW_VTlib_vers;
extern const char *BKW_VTlib_date;

extern "C" { // DataBase functions
    char *EOS_VT_BKW_Init();
    char *EOS_VT_BKW_Info();
    void *EOS_VT_BKW_Constructor();
}
//
//
class EOS;
class BKW;
//
//
class BKW_VT : public EOS_VT
{
private:
    void operator=(const BKW_VT&);              // disallowed
    BKW_VT(const BKW_VT&);                      // disallowed
    BKW_VT *operator &();                       // disallowed, use Duplicate()
protected:
    EOS *Ve();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    BKW_VT();
    ~BKW_VT();
    BKW_VT(BKW &eos);
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

#endif // EOSLIB_BKW_VT_H
