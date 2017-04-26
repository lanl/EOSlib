#ifndef EOSLIB_ELASTIC_TYPES_H
#define EOSLIB_ELASTIC_TYPES_H

#include "Elastic.h"
//
#define Shear1_vers "Shear1v2.0.1"
#define Shear1_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *Shear1lib_vers;
extern const char *Shear1lib_date;
//
extern "C" { // DataBase functions
    char *EOS_VT_Shear1_Init();
    char *EOS_VT_Shear1_Info();
    void *EOS_VT_Shear1_Constructor();
}
//
//
class Shear1 : public Elastic_VT
{
// Fshear=0.5*V_ref*G*(I1*I3^{-1/3} -3)
//  I1 = (V/V_ref)^(2/3) * [ exp(4/3*eps_el) + 2*exp(-2/3*eps_el) ]
//  I3 = (V/V_ref)^2
private:
    Shear1(const Shear1&);                      // for use by Duplicate only    
    void operator=(const Shear1&);              // disallowed
    Shear1 *operator &();                       // disallowed, use Duplicate()
protected:
    double G;                                   // shear Modulus
 //
    int PreInit(Calc &);
    int PostInit();
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    Shear1();
    ~Shear1();
    //Shear1 *Duplicate()     { return (Shear1*)EOSbase::Duplicate();}
    //using Elastic_VT::Duplicate;
    //Elastic_VT *Duplicate()     { return Elastic_VT::Duplicate();}  
    ExtEOS_VT  *Duplicate(const double *z);

    double Fshear(double V, double T, double eps_el);
    double Sshear(double V, double T, double eps_el);
 // eshear = Fshear + T*Sshear
    double eshear(double V, double T, double eps_el);
    double Pshear(double V, double T, double eps_el);
    double  shear(double V, double T, double eps_el);
    double dshear(double V, double e, double eps_el); // (d/deps_el)shear
 // stress (xx component) =  Pshear + (2/3)*shear
    double stress  (double V, double T, double eps_el);
 // stress_V = (d/dV) stress
    double stress_V(double V, double T, double eps_el);
 // stress_T = (d/dT) stress
    double stress_T(double V, double T, double eps_el);
 // CVshear = (d/dT) eshear
    double  CVshear(double V, double T, double eps_el);
};

#endif // EOSLIB_ELASTIC_TYPES_H
