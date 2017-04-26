#ifndef EOSLIB_IDEAL_GAS_H
#define EOSLIB_IDEAL_GAS_H

#define IdealGas_vers "IdealGasv2.0.1"
#define IdealGas_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *IdealGaslib_vers;
extern const char *IdealGaslib_date;

extern "C" { // DataBase functions
    char *EOS_IdealGas_Init();
    char *EOS_IdealGas_Info();
    void *EOS_IdealGas_Constructor();
}
//
//
class EOS_VT;
class IdealGas_VT;
//
//
class IdealGas : public EOS
{
    friend class IdealGas_VT;
private:
    void operator=(const IdealGas&);        // disallowed
    IdealGas(const IdealGas&);              // disallowed
    IdealGas *operator &();                 // disallowed, use Duplicate()
public:
// parameters
    double gamma;
    double Cv;
protected:
    EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    IdealGas();
    IdealGas(double g, double C_v = 1.);
    ~IdealGas();
    IdealGas(IdealGas_VT &gas);
// EOS functions
    double P(double V, double e);           // Pressure
    double T(double V, double e);           // Temperature
    double S(double V, double e);           // Entropy
    
    double c2(double V, double e);          // sound speed squared
    double Gamma(double V, double e);       // Gruneisen coefficient
    double CV(double V, double e);         // Specific heat
    double FD(double V, double e);          // Fundamental Derivative

    Isentrope    *isentrope   (const HydroState &state);
    Hugoniot     *shock       (const HydroState &state);
    Isotherm     *isotherm    (const HydroState &state);
    Detonation   *detonation  (const HydroState &state, double p0);
    Deflagration *deflagration(const HydroState &state, double p0);
    
    int PT(double P, double T, HydroState &state);
    double e_PV(double p, double V);
    int NotInDomain(double V, double e);
};

#endif // EOSLIB_IDEAL_GAS_H
