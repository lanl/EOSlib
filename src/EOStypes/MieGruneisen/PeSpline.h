#ifndef EOSLIB_PE_SPLINE_EOS_H
#define EOSLIB_PE_SPLINE_EOS_H

#include <EOS.h>
#include <PeSplineParams.h>

#define PeSpline_vers "PeSplinev2.0.1"
#define PeSpline_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

using namespace std;

extern const char *PeSplinelib_vers;
extern const char *PeSplinelib_date;

extern "C" { // DataBase functions
    char *EOS_PeSpline_Init();
    char *EOS_PeSpline_Info();
    void *EOS_PeSpline_Constructor();
}
//
//
class EOS_VT;
class PeSpline_VT;
//
//
class PeSpline : public EOS, public PeSplineParams
{
private:
    void operator=(const PeSpline&);        // disallowed
    PeSpline(const PeSpline&);              // disallowed
    PeSpline *operator &();                 // disallowed, use Duplicate()
protected:
    EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
public:
    PeSpline();
    PeSpline(PeSpline_VT &eos);
    ~PeSpline();    
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

#endif // EOSLIB_PE_SPLINE_EOS_H
