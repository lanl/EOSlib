#ifndef EOSLIB_HAYESBM_EOS_H
#define EOSLIB_HAYESBM_EOS_H

#include "HayesBMparams.h"

#define HayesBM_vers "HayesBMv2.0.1"
#define HayesBM_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *HayesBMlib_vers;
extern const char *HayesBMlib_date;

extern "C" { // DataBase functions
    char *EOS_HayesBM_Init();
    char *EOS_HayesBM_Info();
    void *EOS_HayesBM_Constructor();
}
//
//
class EOS_VT;
class HayesBM_VT;
//
//
class HayesBM : public EOS, public HayesBMparams
{
private:
    void operator=(const HayesBM&);       // disallowed
    HayesBM(const HayesBM&);              // disallowed
    HayesBM *operator &();                // disallowed, use Duplicate()
protected:
    EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    HayesBM();
    HayesBM(HayesBM_VT &eos);
    ~HayesBM();
    
// EOS functions
    double P(double V, double e);       // Pressure
    double T(double V, double e);       // Temperature
    double S(double V, double e);       // Entropy
    
    double c2(double V, double e);      // sound speed squared
    double Gamma(double V, double e);   // Gruneisen coefficient
    double CV(double V, double e);      // Specific heat
    double FD(double V, double e);      // Fundamental Derivative
    double e_PV(double p, double V);

    //Isotherm *isotherm(const HydroState &state);
    //int PT(double P, double T, HydroState &state);
    int NotInDomain(double V, double e);

// special to HayesBM EOS
    double     P(double V) { return HayesBMparams::P(V);    }
    double     e(double V) { return HayesBMparams::e(V);    }
    double Gamma(double V) { return HayesBMparams::Gamma(V);}
    double P_VT(double V, double T);
    double e_VT(double V, double T);
    //double V_PT(double P, double T);
};

inline double HayesBM::P_VT(double V, double T)
{
	return P(V) + Cv*(T-T0)*Gamma(V)/V;
}
inline double HayesBM::e_VT(double V, double T)
{
	return e(V) + Cv*(T-T0)-Cv*T0*Gp(V);
}
/**************
inline double HayesBM::V_PT(double P, double T)
{
    return NaN;
}
****************/

#endif // EOSLIB_HAYESBM_EOS_H
