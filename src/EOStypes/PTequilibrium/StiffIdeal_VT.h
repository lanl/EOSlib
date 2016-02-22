#ifndef EOSLIB_STIFF_IDEAL_VT_H
#define EOSLIB_STIFF_IDEAL_VT_H

#include <EOS_VT.h>

#define StiffIdeal_VT_vers "StiffIdeal_VTv2.0.1"
#define StiffIdeal_VT_date "Jan. 5, 2013"

#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *StiffIdeal_VTlib_vers;
extern const char *StiffIdeal_VTlib_date;

extern "C" { // DataBase functions
    char *EOS_VT_StiffIdeal_Init();
    char *EOS_VT_StiffIdeal_Info();
    void *EOS_VT_StiffIdeal_Constructor();
}
//
//
class EOS;
class StiffIdeal;
//
//  PT equilibrium of Stiffened gas and Ideal gas
//  model for solid explosive
//    reactants = stiffened gas
//    products  = ideal gas
//
class StiffIdeal_VT : public EOS_VT
{
private:
    void operator=(const StiffIdeal_VT&);   // disallowed
    StiffIdeal_VT(const StiffIdeal_VT&);    // disallowed
    StiffIdeal_VT *operator &();            // disallowed, use Duplicate()
protected:
    int Pequil(double V, double T);        // equilibrate P
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
    EOS *Ve();
public:
    // Stiffened gas parameters
    //    P = G1*Cv1*T/V - P_p
    //    e = Cv1*T +P_p*V
    double G1;    // Gruneisen coefficient
    double P_p;
    double Cv1;    
    // Ideal gas parameters
    //    P = G2*Cv2*T/V
    //    e = Cv2*T-Q
    double G2;              // gamma -1
    double Q;               // heat release for reactants -> products
    double Cv2;
    //
    double P_ref, e_ref;
    double dS;              // S_products - S_reactants at (V_ref,e_ref) 
    // 
    double lambda1;         // mass fraction of stiffened gas
    double lambda2;         // mass fraction of ideal gas
    // set by PTequil
    double V1, V2, Vav;     // Vav = lambda1*V1 + lambda2*V2
    double e1, e2, eav;     // eav = lambda1*e1 + lambda2*e2
    double Peq, Teq;        // Peq = P1(V1,T) = P2(V2,T)
    StiffIdeal_VT();
    ~StiffIdeal_VT();
    StiffIdeal_VT(StiffIdeal &eos);
// EOS functions
    double P(double V, double T);               // Pressure
    double e(double V, double T);               // specific energy
    double S(double V, double T);               // entropy
    double P_V(double V, double T);             // dP/dV @ const T
    double P_T(double V, double T);             // dP/dT @ const V
    double CV(double V, double T);              // T*(d/dT)S
    //
    double Gamma(double V, double T);           // -(V/T)dT/dV @ const S
    double c2(double V, double T);              // isentropic (sound speed)^2
    int NotInDomain(double V, double T);
};

#endif // EOSLIB_STIFF_IDEAL_VT_H
