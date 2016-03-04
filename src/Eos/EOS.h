#ifndef EOSLIB_EOS_H
#define EOSLIB_EOS_H
//
#include "EOSbase.h"
#include "States.h"
#include "IO.h"
#include "Wave.h"
#include <String.h>
//
#define EOS_vers "EOSv2.0.1"
#define EOS_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *EOSlib_vers;
extern const char *EOSlib_date;

inline int EOSlib_mismatch() { return strcmp(EOS_vers, EOSlib_vers); }
inline int EOSlib_mismatch(const char *vers) {return strcmp(vers, EOSlib_vers);}
//
class EOS_tol;
class EOS;
//
extern "C" { // dynamic link functions for database
    char *EOS_Init();
    char *EOS_Info();
}
//
class EOS_VT;
//
class EOS_tol                               // tolerances used by EOS
{
public:
 // dimensionless
    double dV_factor;                       // default 1.e-3
    double de_factor;                       // default 1.e-2
    double c2_tol;                          // default 1.e-3
    double Gamma_tol;                       // default 1.e-3
    double CV_tol;                          // default 1.e-3
    double FD_tol;                          // default 1.e-3
    double OneDFunc_rel_tol;                // default 1.e-8
    double OneDFunc_abs_tol;                // default 1.e-14
    double ODE_epsilon;                     // default 1.e-8
    int    Iter_max;                        // default 20
 // dimensional
    double P_vac;                           // default 1.e-6
 //
    EOS_tol();      // units for dimensional quantities ?
    int Convert_tol(Convert&);
    void InitParams(Calc&);
};
//
//
//
class EOS : public EOSbase, public EOS_tol      // abstract base class
{
public:
    static int  init;
    static void Init();
 // reference state
    double V_ref;
    double e_ref;
 // misc
    EOS_VT *eosVT();
    virtual ~EOS();
    EOS *Duplicate()    { return static_cast<EOS*>(EOSbase::Duplicate());} 
    virtual void SetVerbose(int d);
private:
    EOS();                                  // disallowed
    EOS *operator &();                      // disallowed, use Duplicate()
    void operator=(const EOS&);             // disallowed
protected:
    EOS_VT *VTeos;
    virtual EOS_VT *VT();
    void Transfer(EOS_VT &eos);
 //
    EOS(const char *eos_type);              // eos_type must not be NULL    
    EOS(const EOS&);
    int Copy(const EOS&, int check = 1);
    int InitBase(Parameters &, Calc &, DataBase *db=NULL);
    virtual int InitParams(Parameters &, Calc &, DataBase *db=NULL) = 0;
    virtual void  PrintParams(ostream &out);
    virtual int ConvertParams(Convert &convert);
public:
 // function *** mandatory ***
    virtual double P(double V, double e)         = 0;   // Pressure
        // P = NaN if outside domain of EOS
 // functions (optional)        
    virtual double T(double V, double e);       // Temperature:   default NaN
    virtual double S(double V, double e);       // Entropy:       default NaN
 // functions (recommended), default is numerical derivative
    virtual double c2(double V, double e);      // sound speed squared
    virtual double Gamma(double V, double e);   // Gruneisen coefficient
    virtual double CV(double V, double e);      // Specific heat
        //   c^2 = dP/d(rho) |_S = gamma P V
        // Gamma = V dP/de|_V
        //   C_V = de/dT |_V
        // Thermodynamic relations
        //   de =     -P dV   +       T dS
        // V dP =   -c^2 dV/V + Gamma T dS
        // dT/T = -Gamma dV/V +         dS/C_V
    virtual double FD(double V, double e);
        // Fund. Derivative, FD = (1/c)*d(rho c)/d(rho)|_S
        // on isentrope
        // P(V) = P_0 + rho_0*c_0^2 {-(V-V_0)/V_0 + [(V-V_0)/V_0]^2 FD + . . .}
 // functions (provided)
    virtual Isentrope *isentrope(const HydroState &state);
        // default: integrates using ODE
    virtual Hugoniot *shock(const HydroState &state);
        // default: finds zero using OneDFunction
    virtual Isotherm *isotherm(const HydroState &state);
        // default: integrates using ODE
    virtual Detonation *detonation(const HydroState &state, double p0);
        // default: integrate (de/dV)_h
    virtual Deflagration *deflagration(const HydroState &state, double p0);
        // default: integrate (de/dV)_h
    virtual int PT(double P, double T, HydroState &state);
    virtual double e_PV(double p, double V);
    virtual int NotInDomain(double V, double e);
    int InDomain(double V, double e) { return !NotInDomain(V,e); }
 //    
    virtual double CP(double V, double e);      // Specific heat at constant P
    virtual double KT(double V, double e);      // Isothermal bulk modulus
    virtual double beta(double V, double e);    // volumetric thermal expansion
    double KS(double V, double e);              // Isentropic bulk modulus
    double  c(double V, double e);              // sound speed = sqrt(c2)
 //
    int Evaluate(const HydroState &state1, HydroThermal &state2);
    int Evaluate(const HydroState &state1, WaveState    &state2);
    int Evaluate(const HydroState &state1, ThermalState &state2);
 //
    double     P(const HydroState &state) {return     P(state.V,state.e);}
    double    c2(const HydroState &state) {return    c2(state.V,state.e);}
    double Gamma(const HydroState &state) {return Gamma(state.V,state.e);}
    double     T(const HydroState &state) {return     T(state.V,state.e);}
    double     S(const HydroState &state) {return     S(state.V,state.e);}
    double     c(const HydroState &state) {return     c(state.V,state.e);}
    double    CV(const HydroState &state) {return    CV(state.V,state.e);}
    double    CP(const HydroState &state) {return    CP(state.V,state.e);}
    double    KS(const HydroState &state) {return    KS(state.V,state.e);}
    double    KT(const HydroState &state) {return    KT(state.V,state.e);}
    double  beta(const HydroState &state) {return  beta(state.V,state.e);}
};
//
inline int deleteEOS(EOS *&eos)
{
    int status = deleteEOSbase(eos,"EOS");
    eos = NULL;
    return status;
}
//
// Database
// 
inline EOS *FetchEOS(const char *type, const char *name, DataBase &db)
            { return (EOS *) db.FetchObj("EOS",type,name);}
inline EOS *FetchNewEOS(const char *type, const char *name, DataBase &db)
            { return (EOS *) db.FetchNewObj("EOS",type,name);}
inline EOS *FetchEOS(const char *type_name, DataBase &db)
            { return (EOS *) db.FetchObj("EOS",type_name);}
inline EOS *FetchNewEOS(const char *type_name, DataBase &db)
            { return (EOS *) db.FetchNewObj("EOS",type_name);}
// print
inline ostream & operator<<(ostream &out, EOS &eos)
            { return eos.Print(out); }
//
// inline EOS functions
//
inline double EOS::c(double V, double e)
{
    double csq = c2(V,e);
    return (std::isnan(csq) || csq < 0) ? EOS::NaN : sqrt(csq);
}
inline double EOS::KS(double V, double e)
{
    return c2(V,e)/V;
}

#endif // EOSLIB_EOS_H
