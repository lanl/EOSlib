#ifndef EOSLIB_USUP_EOS_H
#define EOSLIB_USUP_EOS_H

#include <EOS.h>
#include <UsUpParams.h>

#define UsUp_vers "UsUpv2.0.1"
#define UsUp_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

using namespace std;

extern const char *UsUplib_vers;
extern const char *UsUplib_date;

extern "C" { // DataBase functions
    char *EOS_UsUp_Init();
    char *EOS_UsUp_Info();
    void *EOS_UsUp_Constructor();
}
//
//
class EOS_VT;
class UsUp_VT;
//
//
class UsUp : public EOS, public UsUpParams
{
private:
    void operator=(const UsUp&);        // disallowed
    UsUp(const UsUp&);                  // disallowed
    UsUp *operator &();                 // disallowed, use Duplicate()
protected:
    EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
public:
    UsUp();
    UsUp(UsUp_VT &eos);
    ~UsUp();    
// EOS functions
    double P(double V, double e);       // Pressure
    double T(double V, double e);       // Temperature
    double S(double V, double e);       // Entropy
    
    double c2(double V, double e);      // sound speed squared
    double Gamma(double V, double e);   // Gruneisen coefficient
    double CV(double V, double e);      // Specific heat
    double FD(double V, double e);      // Fundamental Derivative
    double e_PV(double p, double V);

    int NotInDomain(double V, double e);
};

#endif // EOSLIB_USUP_EOS_H
