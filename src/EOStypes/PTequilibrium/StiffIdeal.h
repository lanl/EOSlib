#ifndef EOSLIB_STIFF_IDEAL_EOS_H
#define EOSLIB_STIFF_IDEAL_EOS_H

#include <EOS.h>

#define StiffIdeal_vers "StiffIdealv2.0.1"
#define StiffIdeal_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *StiffIdeallib_vers;
extern const char *StiffIdeallib_date;

extern "C" { // DataBase functions
    char *EOS_StiffIdeal_Init();
    char *EOS_StiffIdeal_Info();
    void *EOS_StiffIdeal_Constructor();
}
//
//
class EOS_VT;
class StiffIdeal_VT;
//
//  PT equilibrium of Stiffened gas and Ideal gas
//  model for solid explosive
//    reactants = stiffened gas
//    products  = ideal gas
//
class StiffIdeal : public EOS
{
private:
    void operator=(const StiffIdeal&);      // disallowed
    StiffIdeal *operator &();               // disallowed, use Duplicate()
protected:
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
    EOS_VT *VT();
public:
    // Stiffened gas parameters
    //    P1 = G1*e1/V1 -(G1+1)*P_p
    //    T1 = (e1-P_p*V1)/Cv1
    double G1;    // Gruneisen coefficient
    double P_p;
    double Cv1;    
    // Ideal gas parameters
    //    P2 = G2*(e2+Q)/V2
    //    T2 = (e2+Q)/Cv2
    double G2;   // gamma -1
    double Q;    // heat release for reactants -> products
    double Cv2;
    //
    double dS;   // S_products - S_reactants at (V_ref,e_ref) 
    // 
    double lambda1;     // mass fraction of stiffened gas
    double lambda2;     // mass fraction of ideal gas
    // set by PTequil
    double V1, V2, Vav; // Vav = lambda1*V1 + lambda2*V2
    double e1, e2, eav; // eav = lambda1*e1 + lambda2*e2
    double Peq;         // Peq = P1(V1,e1) = P2(V2,e2)
    double Teq;         // Teq = T1(V1,e1) = T2(V2,e2)
    StiffIdeal();
    StiffIdeal(const StiffIdeal&);
    StiffIdeal(StiffIdeal_VT &eos);
    ~StiffIdeal();    
    StiffIdeal *Duplicate()
        { return static_cast<StiffIdeal*>(EOSbase::Duplicate());  }
// EOS functions
    double P(double V, double e);       // Pressure
    double T(double V, double e);       // Temperature
    double S(double V, double e);       // Entropy
    //
    double c2(double V, double e);      // sound speed squared
    double Gamma(double V, double e);   // Gruneisen coefficient
    double CV(double V, double e);      // Specific heat
    double FD(double V, double e);      // Fundamental Derivative
    //
    int NotInDomain(double V, double e);
    int PT(double P, double T, HydroState &state);
    // ToDo: Add wave routines for degenerate gases lambda1 = 0 or 1
    //Isentrope *isentrope(const HydroState &state);
    //Hugoniot  *shock    (const HydroState &state);
    //Isotherm  *isotherm (const HydroState &state);    
// StiffIdeal functions
    void set_lambda1(double lambda)
        { lambda1 = lambda; lambda2 = 1.-lambda; Vav = -1.;}
    void set_lambda2(double lambda)
        { lambda2 = lambda; lambda1 = 1.-lambda; Vav = -1.;}
    int PTequil(double V, double e);        // equilibrate P and T
    int ddlambda(double V, double e, double &dP, double &dT);
        // dP = d(Peq)/d(lambda1) and dT = d(Teq)/d(lambda1)
};

#endif // EOSLIB_STIFF_IDEAL_EOS_H
