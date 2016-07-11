#ifndef EOSLIB_PT_EQUILIBIUM_VT_H
#define EOSLIB_PT_EQUILIBIUM_VT_H

#include <EOS_VT.h>

#define PTequilibrium_VT_vers "PTequilibrium_VTv2.0.1"
#define PTequilibrium_VT_date "Jan. 5, 2013"

#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

using namespace std;

extern const char *PTequilibrium_VTlib_vers;
extern const char *PTequilibrium_VTlib_date;

extern "C" { // DataBase functions
    char *EOS_VT_PTequilibrium_Init();
    char *EOS_VT_PTequilibrium_Info();
    void *EOS_VT_PTequilibrium_Constructor();
}
//
//
class EOS;
class PTequilibrium;
//
//
class PTequilibrium_VT : public EOS_VT
{
// P(V, T) = P1(V1,T) = P2(V2,T)
// e(V, T) = lambda1*e1(V1,T) + lambda2*(e2(V2,T)-de)
// where
//      1 = lambda1    + lambda2
//      V = lambda1*V1 + lambda2*V2
// de is shift in energy origin between reactants and products
// or energy released by reaction
//      material 1 -> material 2
//      lambda2 is reaction progress variable
// if lambda2=0, then e1 = e,     all reactants
// if lambda2=1, then e2 = e +de, all products
private:
    void operator=(const PTequilibrium_VT&);    // disallowed
    PTequilibrium_VT(const PTequilibrium_VT&);  // disallowed
    PTequilibrium_VT *operator &();             // disallowed, use Duplicate()
protected:
    EOS *Ve();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    double tol;     // rel error for P iteration
    EOS_VT *eos1;
    EOS_VT *eos2;
    double de;              // de = e2_origin -e1_origin   
    double lambda1; // mass fraction of eos1
    double lambda2; // mass fraction of eos2
                    // 1 = lambda1 + lambda2
    double V1;
    double V2;
    int count;      // diagnostic, count of iterations
    // V = lambda1*V1 + (1-lambda1)*V2
    // eos1->P(V1,T) = eos2->P(V2,T)
    PTequilibrium_VT();
    ~PTequilibrium_VT();
    PTequilibrium_VT(PTequilibrium &eos);
    ostream &PrintComponents(ostream &out);
// EOS functions
    double P(double V, double T);               // Pressure
    double e(double V, double T);               // specific energy
    double S(double V, double T);               // entropy
    double P_V(double V, double T);             // dP/dV @ const T
    double P_T(double V, double T);             // dP/dT @ const V
    double CV(double V, double T);              // T*(d/dT)S @ const V
    //
    double Gamma(double V, double T);           // -(V/T)dT/dV @ const S
    double c2(double V, double T);              // isentropic (sound speed)^2
    int NotInDomain(double V, double T);
};

#endif // EOSLIB_PT_EQUILIBIUM_VT_H

/**************
Add functionality
double VPT(P,T) return V(P,T)  // integrate P_V dV at const T
void set_lambda1(lambda)
void set_lambda2(lambda)
int ddlambda1(V,T,dV1,dV2,dP)
  returns dP  = (V2-V1)* P1_V * P2_V /(lambda1*dP2_V + lambda2*dP1_V
          dV2 = (V2-V1)*dP1_V / (lambda1*dP2_V + lambda2*dP1_V)
          dV1 = (V2-V1)*dP2_V / (lambda1*dP2_V + lambda2*dP1_V)
*************/
