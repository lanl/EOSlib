#ifndef EOSLIB_STIFFENED_GAS_H
#define EOSLIB_STIFFENED_GAS_H

#include "StiffGasParams.h"

#define StiffenedGas_vers "StiffenedGasv2.0.1"
#define StiffenedGas_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *StiffenedGaslib_vers;
extern const char *StiffenedGaslib_date;

extern "C" { // DataBase functions
    char *EOS_StiffenedGas_Init();
    char *EOS_StiffenedGas_Info();
    void *EOS_StiffenedGas_Constructor();
}
//
//
class EOS_VT;
class StiffenedGas_VT;
//
//
class StiffenedGas : public EOS, public StiffGasParams
{
private:
    void operator=(const StiffenedGas&);    // disallowed
    StiffenedGas(const StiffenedGas&);      // disallowed
    StiffenedGas *operator &();             // disallowed, use Duplicate()
protected:
    EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    StiffenedGas();
    StiffenedGas(double g, double e, double p, double C_v = 1.);
    ~StiffenedGas();
    StiffenedGas(StiffenedGas_VT &gas);
// EOS functions
    double P(double V, double e);           // Pressure
    double T(double V, double e);           // Temperature
    double S(double V, double e);           // Entropy
    
    double c2(double V, double e);          // sound speed squared
    double Gamma(double V, double e);       // Gruneisen coefficient
    double CV(double V, double e);          // Specific heat
    double FD(double V, double e);          // Fundamental Derivative
    int NotInDomain(double V, double e);

    Isentrope *isentrope(const HydroState &state);
    Hugoniot  *shock(const HydroState &state);
    Isotherm *isotherm(const HydroState &state);
    int PT(double P, double T, HydroState &state);
    double e_PV(double p, double V);
};

#endif // EOSLIB_STIFFENED_GAS_H
