#ifndef EOSLIB_PE_SPLINE_VT_H
#define EOSLIB_PE_SPLINE_VT_H

#include <EOS_VT.h>
#include <PeSplineParams.h>

#define PeSpline_VT_vers "PeSpline_VTv2.0.1"
#define PeSpline_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *PeSpline_VTlib_vers;
extern const char *PeSpline_VTlib_date;

extern "C" { // DataBase functions
    char *EOS_VT_PeSpline_Init();
    char *EOS_VT_PeSpline_Info();
    void *EOS_VT_PeSpline_Constructor();
}
//
//
class EOS;
class PeSpline;
//
//
class PeSpline_VT : public EOS_VT, public PeSplineParams
{
private:
    void operator=(const PeSpline_VT&);         // disallowed
    PeSpline_VT(const PeSpline_VT&);            // disallowed
    PeSpline_VT *operator &();                  // disallowed, use Duplicate()
protected:
    EOS *Ve();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    PeSpline_VT();
    ~PeSpline_VT();
    PeSpline_VT(PeSpline &eos);
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

#endif // EOSLIB_PE_SPLINE_VT_H
