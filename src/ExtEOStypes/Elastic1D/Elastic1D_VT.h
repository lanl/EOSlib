#ifndef EOSLIB_ELASTIC1D_VT_H
#define EOSLIB_ELASTIC1D_VT_H

#include <Shear1D.h>
class Elastic1D;
//
#define Elastic1D_VT_vers "Elastic1D_VTv2.0.1"
#define Elastic1D_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *Elastic1D_VTlib_vers;
extern const char *Elastic1D_VTlib_date;
//
extern "C" { // DataBase functions
    char *EOS_VT_Elastic1D_Init();
    char *EOS_VT_Elastic1D_Info();
    void *EOS_VT_Elastic1D_Constructor();
}
//
// Base class extending EOS_VT for 1D elastic flow
class Elastic1D_VT : public EOS_VT
{
private:
    void operator=(const Elastic1D_VT&);        // disallowed
    Elastic1D_VT *operator &();                 // disallowed, use Duplicate()
public:
    EOS_VT  *hydro;
    Shear1D *shear;
    double  eps_el_ref;
protected:
    virtual EOS *Ve();
    virtual int InitParams(Parameters &, Calc &, DataBase *db);
    virtual void PrintParams(ostream &out);
    virtual int ConvertParams(Convert &convert);    
public:
    Elastic1D_VT(const char *ptype = "Elastic1D");
    Elastic1D_VT(Elastic1D_VT&);
    Elastic1D_VT(Elastic1D&);
    Elastic1D_VT *Duplicate();
    ~Elastic1D_VT();
   ostream &PrintComponents(ostream &out);
// EOS_VT functions
    virtual double P(double V, double T);
    virtual double e(double V, double T);
    virtual double S(double V, double T);
    virtual double P_V(double V, double T);
    virtual double P_T(double V, double T);
    virtual double CV(double V, double T);
//
    virtual double F(double V, double T);
    virtual int NotInDomain(double V, double T);
    //virtual double CP(double V, double T);
    //virtual double KS(double V, double T);
    //virtual double KT(double V, double T);
    //virtual double Gamma(double V, double T);
    //virtual double beta(double V, double T);
    //virtual double c2(double V, double T);
    //virtual double cT2(double V, double T);
// elastic EOS_VT functions
    virtual double P(double V, double T, double eps_el);
    virtual double e(double V, double T, double eps_el);
    virtual double S(double V, double T, double eps_el);
    virtual double P_V(double V, double T, double eps_el);
    virtual double P_T(double V, double T, double eps_el);
    virtual double CV(double V, double T, double eps_el);
    virtual double F(double V, double T, double eps_el);
    virtual int NotInDomain(double V, double T, double eps_el);
// elastic functions
    double Eps_el(double V) { return log(V/V_ref)-eps_el_ref;}
};

inline Elastic1D_VT *Elastic1D_VT::Duplicate()
{ return static_cast<Elastic1D_VT*>(EOSbase::Duplicate()); }

#endif // EOSLIB_ELASTIC1D_VT_H
