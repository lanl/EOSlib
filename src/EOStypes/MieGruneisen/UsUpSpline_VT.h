#ifndef EOSLIB_USUP_SPLINE_VT_H
#define EOSLIB_USUP_SPLINE_VT_H

#include <EOS_VT.h>
#include <UsUpSplineParams.h>

#define UsUpSpline_VT_vers "UsUpSpline_VTv2.0.1"
#define UsUpSpline_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *UsUpSpline_VTlib_vers;
extern const char *UsUpSpline_VTlib_date;

extern "C" { // DataBase functions
    char *EOS_VT_UsUpSpline_Init();
    char *EOS_VT_UsUpSpline_Info();
    void *EOS_VT_UsUpSpline_Constructor();
}
//
//
class EOS;
class UsUpSpline;
//
//
class UsUpSpline_VT : public EOS_VT, public UsUpSplineParams
{
private:
    void operator=(const UsUpSpline_VT&);       // disallowed
    UsUpSpline_VT(const UsUpSpline_VT&);        // disallowed
    UsUpSpline_VT *operator &();                // disallowed, use Duplicate()
protected:
    EOS *Ve();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    UsUpSpline_VT();
    ~UsUpSpline_VT();
    UsUpSpline_VT(UsUpSpline &eos);
// EOS functions
    double P(double V, double T);               // Pressure
    double e(double V, double T);               // specific energy
    double S(double V, double T);               // entropy
    double P_V(double V, double T);             // dP/dV @ const T
    double P_T(double V, double T);             // dP/dT @ const V
    double CV(double V, double T);              // T*(d/dT)S
    //
    double Gamma(double V, double T);           // -(V/T)dT/dV @ const S
    double c2(double V, double T);              // isothermal (sound speed)^2
    int NotInDomain(double V, double T);
};

#endif // EOSLIB_USUP_SPLINE_VT_H
