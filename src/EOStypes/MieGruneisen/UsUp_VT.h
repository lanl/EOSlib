#ifndef EOSLIB_USUP_VT_H
#define EOSLIB_USUP_VT_H

#include <EOS_VT.h>
#include <UsUpParams.h>

#define UsUp_VT_vers "UsUp_VTv2.0.1"
#define UsUp_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *UsUp_VTlib_vers;
extern const char *UsUp_VTlib_date;

extern "C" { // DataBase functions
    char *EOS_VT_UsUp_Init();
    char *EOS_VT_UsUp_Info();
    void *EOS_VT_UsUp_Constructor();
}
//
//
class EOS;
class UsUp;
//
//
class UsUp_VT : public EOS_VT, public UsUpParams
{
private:
    void operator=(const UsUp_VT&);             // disallowed
    UsUp_VT(const UsUp_VT&);                    // disallowed
    UsUp_VT *operator &();                      // disallowed, use Duplicate()
protected:
    EOS *Ve();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    UsUp_VT();
    ~UsUp_VT();
    UsUp_VT(UsUp &eos);
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
};

#endif // EOSLIB_USUP_VT_H
