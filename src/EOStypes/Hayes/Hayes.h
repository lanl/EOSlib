#ifndef EOSLIB_HAYES_EOS_H
#define EOSLIB_HAYES_EOS_H

#include "HayesParams.h"

#define Hayes_vers "Hayesv2.0.1"
#define Hayes_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *Hayeslib_vers;
extern const char *Hayeslib_date;

extern "C" { // DataBase functions
    char *EOS_Hayes_Init();
    char *EOS_Hayes_Info();
    void *EOS_Hayes_Constructor();
}
//
//
class EOS_VT;
class Hayes_VT;
//
//
class Hayes : public EOS, public HayesParams
{
private:
    void operator=(const Hayes&);       // disallowed
    Hayes(const Hayes&);                // disallowed
    Hayes *operator &();                // disallowed, use Duplicate()
protected:
    EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    Hayes();
    Hayes(Hayes_VT &eos);
    ~Hayes();
    
// EOS functions
    double P(double V, double e);       // Pressure
    double T(double V, double e);       // Temperature
    double S(double V, double e);       // Entropy
    
    double c2(double V, double e);      // sound speed squared
    double Gamma(double V, double e);   // Gruneisen coefficient
    double CV(double V, double e);      // Specific heat
    double FD(double V, double e);      // Fundamental Derivative

    Isotherm *isotherm(const HydroState &state);
    int PT(double P, double T, HydroState &state);
    double e_PV(double p, double V);
    int NotInDomain(double V, double e);

// special to Hayes EOS
    double P_VT(double V, double T);
    double e_VT(double V, double T);
    double V_PT(double P, double T);
};

inline double Hayes::P_VT(double V, double T)
{
    return P0 + Gamma0/V0*Cv*(T-T0) + K0/N*(pow(V0/V,N)-1);
}
inline double Hayes::e_VT(double V, double T)
{
    return e0 + P0*(V0-V) + Cv*(T-T0) -Cv*T0*Gamma0*(1-V/V0)
              + K0*V0/N/(N-1)*(pow(V0/V,N-1)-1 - (N-1)*(1-V/V0));
}
inline double Hayes::V_PT(double P, double T)
{
    return V0/pow(1+N/K0*(P-P0-Gamma0/V0*Cv*(T-T0)),1/N);
}


#endif // EOSLIB_HAYES_EOS_H
