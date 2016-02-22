#ifndef EOSLIB_STIFFENED_GAS_VT_H
#define EOSLIB_STIFFENED_GAS_VT_H

#include "StiffGasParams.h"

#define StiffenedGas_VT_vers "StiffenedGas_VTv2.0.1"
#define StiffenedGas_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *StiffenedGas_VTlib_vers;
extern const char *StiffenedGas_VTlib_date;

extern "C" { // DataBase functions
    char *EOS_VT_StiffenedGas_Init();
    char *EOS_VT_StiffenedGas_Info();
    void *EOS_VT_StiffenedGas_Constructor();
}
//
//
class EOS;
class StiffenedGas;
//
//
class StiffenedGas_VT : public EOS_VT, public StiffGasParams
{
    friend class StiffenedGas;
private:
    void operator=(const StiffenedGas_VT&);     // disallowed
    StiffenedGas_VT(const StiffenedGas_VT&);    // disallowed
    StiffenedGas_VT *operator &();              // disallowed, use Duplicate()
protected:
    EOS *Ve();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    StiffenedGas_VT();
    StiffenedGas_VT(double g, double e, double p, double C_v = 1.);
    ~StiffenedGas_VT();
    StiffenedGas_VT(StiffenedGas &gas);
// EOS functions
    double P(double V, double T);               // Pressure
    double e(double V, double T);               // specific energy
    double S(double V, double T);               // entropy
    double P_V(double V, double T);             // dP/dV @ const T
    double P_T(double V, double T);             // dP/dT @ const V
    double CV(double V, double T);              // T*(d/dT)S
    //
    double F(double V, double T);               // F = e - T*S
    int NotInDomain(double V, double T);
    double CP(double V, double T);              // Specific heat at constant P
    double KS(double V, double T);              // Isentropic bulk modulus
    double KT(double V, double T);              // Isothermal bulk modulus
    double Gamma(double V, double T);           // -(V/T)dT/dV @ const S
    double beta(double V, double T);            // volumetric thermal expansion
    double c2(double V, double T);              // isothermal (sound speed)^2
    double cT2(double V, double T);             // isothermal (sound speed)^2
};

#endif // EOSLIB_STIFFENED_GAS_VT_H
