#ifndef EOSLIB_PT_EQUILIBRIUM_EOS_H
#define EOSLIB_PT_EQUILIBRIUM_EOS_H

#include <EOS.h>

#define PTequilibrium_vers "PTequilibriumv2.0.1"
#define PTequilibrium_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *PTequilibriumlib_vers;
extern const char *PTequilibriumlib_date;

extern "C" { // DataBase functions
    char *EOS_PTequilibrium_Init();
    char *EOS_PTequilibrium_Info();
    void *EOS_PTequilibrium_Constructor();
}
//
//
class EOS_VT;
class PTequilibrium_VT;
//
//
class PTequilibrium : public EOS
{
// P(V, e) = P1(V1,e1) = P2(V2,e2)
// T(V, e) = T1(V1,e1) = T2(V2,e2)
// where
//      1 = lambda1    + lambda2
//      V = lambda1*V1 + lambda2*V2
//      e = lambda1*e1 + lambda2*(e2-de)
//      or e+lambda2*de = lambda1*e1 + lambda2*e2
// de is shift in energy origin between reactants and products
// or energy released by reaction
//      material 1 -> material 2
//      lambda2 is reaction progress variable
// if lambda2=0, then P(V,e) = P1(V,e),     all reactants
// if lambda2=1, then P(V,e) = P2(V,e+de),  all products
private:
    void operator=(const PTequilibrium&);       // disallowed
    PTequilibrium *operator &();                // disallowed, use Duplicate()
protected:
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
    EOS_VT *VT();
    virtual int PTiter(double V, double e);     // iteration for PTequilibrium
    int PT0iter(double V, double e);            // typical case
    int PT1iter(double V, double e);            // lambda1 > lambda1_max
    int PT2iter(double V, double e);            // lambda2 > lambda2_max
    void ddVe();            // internal use for equilibrium derivatives
    int NotDomain1(double V,double e);
    int NotDomain2(double V,double e);
public:
    EOS *eos1;
    EOS *eos2;
    double de;              // de = e2_origin -e1_origin   
    double lambda1;         // mass fraction eos1
    double lambda2;         // mass fraction eos2
                            // 1 = lambda1 + lambda2
    double P1_ref, T1_ref;
    double P2_ref, T2_ref;
    // iteration control parameters
    double tol;             // rel error for P,T iteration
    double tol_P;           // abs error for P
    double tol_T;           // abs error for T
    double lambda1_max;     // switch for PT1iter (predominantly eos1)
    double lambda2_max;     // switch for PT2iter (predominantly eos2)
    int max_iter;           // maximum iterations for PTiter0
    int max_iter1;          // maximum iterations for PTiter1 & PTiter2
    // set by PTiter (last solution cached)
    double V1, e1, P1, T1;  // last state1 & initial guess for next iteration
    double c2_1, Gamma1,CV1;
    double V2, e2, P2, T2;  // last state2 & initial guess for next iteration
    double c2_2, Gamma2,CV2;
    double Vav;             // Vav = lambda1*V1 + lambda2*V2
    double eav;             // eav = lambda1*e1 + lambda2*e2
    double Peq;             // Peq = eos1->P(V1,e1) = eos2->P(V2,e2)
    double Teq;             // Teq = eos1->T(V1,e1) = eos2->T(V2,e2)
    // temp variables set by ddVe()
    double dPdV1, dPde1, dPdV2, dPde2;
    double dTdV1, dTde1, dTdV2, dTde2;
    //
    int count;              // diagnostic, count of iterations
    PTequilibrium();
    PTequilibrium(const PTequilibrium&);
    PTequilibrium(PTequilibrium_VT &eos);
    ~PTequilibrium();    
    PTequilibrium *Duplicate()
        { return static_cast<PTequilibrium*>(EOSbase::Duplicate());  }
    ostream &PrintComponents(ostream &out);
// EOS functions
    double P(double V, double e);       // Pressure
    double T(double V, double e);       // Temperature
    double S(double V, double e);       // Entropy
    //
    double c2(double V, double e);      // sound speed squared
    double Gamma(double V, double e);   // Gruneisen coefficient
    double CV(double V, double e);      // Specific heat
    double FD(double V, double e);      // Fundamental Derivative
    double e_PV(double p, double V);
    //
    int NotInDomain(double V, double e);
    int PT(double P, double T, HydroState &state);
    //
    Isentrope *isentrope(const HydroState &state);
    Hugoniot  *shock    (const HydroState &state);
    Isotherm  *isotherm (const HydroState &state);    
// PTequilibrium functions
    void set_lambda1(double lambda);
    void set_lambda1(double lambda, double q);
    void set_lambda2(double lambda);
    void set_lambda2(double lambda, double q);
    int ddlambda(double V, double e, double &dP, double &dT);
        // dP = dPeq/dlambda1 and dT = dTeq/dlambda1
};
//

inline void PTequilibrium::ddVe()
{
    dPde1 = eos1->Gamma(V1,e1)/V1;
    dTde1 = 1./eos1->CV(V1,e1);
    dPdV1 = dPde1*P1 - eos1->c2(V1,e1)/(V1*V1);
    dTdV1 = dTde1*P1 - dPde1*T1;
    //
    dPde2 = eos2->Gamma(V2,e2)/V2;
    dTde2 = 1./eos2->CV(V2,e2);
    dPdV2 = dPde2*P2 - eos2->c2(V2,e2)/(V2*V2);
    dTdV2 = dTde2*P2 - dPde2*T2;
}

#endif // EOSLIB_PT_EQUILIBRIUM_EOS_H
