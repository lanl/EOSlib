#ifndef EOSLIB_ELASTIC1D_H
#define EOSLIB_ELASTIC1D_H

#include <Shear1D.h>
class Elastic1D_VT;
//
#define Elastic1D_vers "Elastic1Dv2.0.1"
#define Elastic1D_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *Elastic1Dlib_vers;
extern const char *Elastic1Dlib_date;
//
extern "C" { // DataBase functions
    char *EOS_Elastic1D_Init();
    char *EOS_Elastic1D_Info();
    void *EOS_Elastic1D_Constructor();
}
//
// Ansatz for Helmholtz free energy (isotropic material)
//   Psi(V,b_el,T) = Psi_hydro(V,T) + Psi_shear(V,invariants(b_el/J^{2/3}),T)
//   ------------------------------------------------------------------------
// where left Cauchy-Green elastic tensor is defined as
//       b_el = F*F^T
// Assume 1-D deformation has form
//    F = (V/V0)^(1/3)*diag[ x_el, 1/x_el^(1/2), 1/x_el^(1/2) ]
// hence
//   b_el(V,eps_el) = (V/V0)^{2/3}*diag[ x_el^2, 1/x_el, 1/x_el ]
// where  x_el = exp(2*eps_el/3)
//   with eps_el scalar elastic-strain variable defined by
//   eps_el(V) = ln(V/V0) - eps_el_ref
// Note, eps_el_ref allows for plastic flow
//   eps_el_ref = 0, corresponds to uniaxial strain
//            F = diag[ V/V_0, 1, 1 ]
//      eps_el ~= V/V0 - 1, negative in compression !
//                          -----------------------
// Cauchy stress is linear combination of I, dev[b_el], dev[(b_el)^{-1}]
// For 1-D deformation
//   dev[b_el]/(V/V0)^{2/3} = (x_el^2-1/x_el^{-1})*diag[2, -1, -1]/3
// Hence dev[Cauchy stress] proportional to diag[2,-1,-1]
//       ------------------------------------------------
// Free energy in 1-D reduces to 
//    Psi(V,eps_el,T) = Psi_hydro(V,T) + Psi_shear(V,eps_el,T)
//    --------------------------------------------------------
// Note, free energy is function of V, T
//       since eps_el(V) = ln(V/V0)-eps_el_ref
//       (d/dV) = (d/dV)_{eps_el} + (1/V)*(d/d eps_el)_V
// Cauchy stress tensor has form
//    sigma  = -(Phydro+Pshear)*I -(Pdev/2)*diag[2,-1,-1]
// where
//    Phydro = -(d/d V)Psi_hydro,            at constant T
//    Pshear = -(d/d V)Psi_shear,            at constant eps_el, T
//      Pdev = -(1/V)*(d/d eps_el)Psi_shear, at constant V, T
// Longitudinal component of stress is
//    sigma^{xx} = -(Phydro+Pshear+Pdev)
// Thermodynamic identity
//    d(e) =  sigma^{xx} d(V) + T d(eta)
//  EOS::P = -sigma^{xx}
//
class Elastic1D : public EOS    // base clasee for 1D elastic flow
{
private:
    void operator=(const Elastic1D&);          // disallowed
    Elastic1D *operator &();                   // disallowed, use Duplicate()
public:
    EOS     *hydro;
    Shear1D *shear;
    double  eps_el_ref;
protected:
    virtual EOS_VT *VT();
    virtual int  InitParams(Parameters &, Calc &, DataBase *db);
    virtual void PrintParams(ostream &out);
    virtual int  ConvertParams(Convert &convert);    
public:
    Elastic1D(const char *ptype = "Elastic1D");
    Elastic1D(Elastic1D&);
    Elastic1D(Elastic1D_VT&);
    Elastic1D *Duplicate();
    virtual ~Elastic1D();
    ostream &PrintComponents(ostream &out);
// EOS functions
    virtual double     P(double V, double e);
    virtual double     T(double V, double e);
    virtual double     S(double V, double e);
    virtual double    c2(double V, double e);
    virtual double Gamma(double V, double e);
    virtual double    CV(double V, double e);
    virtual double    FD(double V, double e);
    virtual int NotInDomain(double V, double e);
    //virtual double    FD(double V, double e);
    //virtual Isentrope       *isentrope(const HydroState &state);
    //virtual Hugoniot            *shock(const HydroState &state);
    //virtual Isotherm         *isotherm(const HydroState &state);
    //virtual Detonation     *detonation(const HydroState &state, double p0);
    //virtual Deflagration *deflagration(const HydroState &state, double p0);
    //virtual int PT(double P, double T, HydroState &state);
    //virtual double   CP(double V, double e);
    //virtual double   KT(double V, double e);
    //virtual double beta(double V, double e);
// elastic EOS functions
    virtual double      P(double V, double e, double eps_el);
    virtual double Phydro(double V, double e, double eps_el);
    virtual double Pshear(double V, double e, double eps_el);
    virtual double   Pdev(double V, double e, double eps_el);
    virtual double      T(double V, double e, double eps_el);
    virtual double      S(double V, double e, double eps_el);
    virtual double     c2(double V, double e, double eps_el);
    virtual double  Gamma(double V, double e, double eps_el);
    virtual double     CV(double V, double e, double eps_el);
    virtual int NotInDomain(double V, double e, double eps_el);
//
    virtual double    CP(double V, double e, double eps_el);
    virtual double    KT(double V, double e, double eps_el);
    virtual double  beta(double V, double e, double eps_el);
    virtual double    FD(double V, double e, double eps_el);
// elastic shear strain
    double Eps_el(double V) { return log(V/V_ref)-eps_el_ref;}
};
inline Elastic1D *Elastic1D::Duplicate()
    { return static_cast<Elastic1D*>(EOSbase::Duplicate());  }  

#endif // EOSLIB_ELASTIC1D_H
