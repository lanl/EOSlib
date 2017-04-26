#ifndef EOSLIB_HAYESBM_VT_H
#define EOSLIB_HAYESBM_VT_H

#include "HayesBMparams.h"

#define HayesBM_VT_vers "HayesBM_VTv2.0.1"
#define HayesBM_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *HayesBM_VTlib_vers;
extern const char *HayesBM_VTlib_date;

extern "C" { // DataBase functions
    char *EOS_VT_HayesBM_Init();
    char *EOS_VT_HayesBM_Info();
    void *EOS_VT_HayesBM_Constructor();
}
//
//
class EOS;
class HayesBM;
//
//
class HayesBM_VT : public EOS_VT, public HayesBMparams
{
    friend class HayesBM;
private:
    void operator=(const HayesBM_VT&);            // disallowed
    HayesBM_VT(const HayesBM_VT&);                // disallowed
    HayesBM_VT *operator &();                     // disallowed, use Duplicate()
protected:
    EOS *Ve();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    HayesBM_VT();
    ~HayesBM_VT();
    HayesBM_VT(HayesBM &eos);
// EOS functions
    double P(double V, double T);               // Pressure
    double e(double V, double T);               // specific energy
    double S(double V, double T);               // entropy
    double P_V(double V, double T);             // dP/dV @ const T
    double P_T(double V, double T);             // dP/dT @ const V
    double CV(double V, double T);              // T*(d/dT)S
    //
    double F(double V, double T);               // F = e - T*S
    double CP(double V, double T);              // Specific heat at constant P
    double KS(double V, double T);              // Isentropic bulk modulus
    double KT(double V, double T);              // Isothermal bulk modulus
    double Gamma(double V, double T);           // -(V/T)dT/dV @ const S
    double beta(double V, double T);            // volumetric thermal expansion
    double c2(double V, double T);              // isothermal (sound speed)^2
    double cT2(double V, double T);             // isothermal (sound speed)^2
    //
    double     P(double V) { return HayesBMparams::P(V);    }
    double     e(double V) { return HayesBMparams::e(V);    }
    double Gamma(double V) { return HayesBMparams::Gamma(V);}
};

#endif // EOSLIB_HAYESBM_VT_H
