#ifndef EOSLIB_USUP_SPLINE_EOS_H
#define EOSLIB_USUP_SPLINE_EOS_H

#include <EOS.h>
#include <UsUpSplineParams.h>

#define UsUpSpline_vers "UsUpSplinev2.0.1"
#define UsUpSpline_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *UsUpSplinelib_vers;
extern const char *UsUpSplinelib_date;

extern "C" { // DataBase functions
    char *EOS_UsUpSpline_Init();
    char *EOS_UsUpSpline_Info();
    void *EOS_UsUpSpline_Constructor();
}
//
//
class EOS_VT;
class UsUpSpline_VT;
//
//
class UsUpSpline : public EOS, public UsUpSplineParams
{
private:
    void operator=(const UsUpSpline&);        // disallowed
    UsUpSpline(const UsUpSpline&);            // disallowed
    UsUpSpline *operator &();                 // disallowed, use Duplicate()
protected:
    EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
public:
    UsUpSpline();
    UsUpSpline(UsUpSpline_VT &eos);
    ~UsUpSpline();    
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

#endif // EOSLIB_USUP_SPLINE_EOS_H
