#ifndef EOSLIB_SHEAR1D_H
#define EOSLIB_SHEAR1D_H

#include <EOS.h>
#include <EOS_VT.h>
//
#define Shear1D_vers "Shear1Dv2.0.1"
#define Shear1D_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *Shear1Dlib_vers;
extern const char *Shear1Dlib_date;
//
inline int Shear1Dlib_mismatch()
    { return strcmp(Shear1D_vers, Shear1Dlib_vers); }
inline int Shear1Dlib_mismatch(const char *vers)
    { return strcmp(vers, Shear1Dlib_vers);         }
//
extern "C" { // dynamic link functions for database
    char *Shear1D_Init();
    char *Shear1D_Info();
    int  Shear1D_Initialize(void *ptr, Parameters &p, DataBase &db);
    void *Shear1D_Duplicate(void *ptr);
    void Shear1D_Destructor(void *ptr);
}
//
// Elastic-Plastic flow
// --------------------
// Ansatz for Helmholtz free energy (isotropic material)
//   Psi(V,b_el,T) = Psi_hydro(V,T) + Psi_shear(V,invariants(b_el/J^{2/3}),T)
//   ------------------------------------------------------------------------
// where left Cauchy-Green elastic tensor is defined as
//       b_el = F*Cp^{-1}*F^T
// and Cp is plastic strain tensor.
// 
// In 1-D, total deformation corresponds to uniaxial strain
//    F = diag[V/V0, 1, 1]
// and plastic strain tensor is assumed to have form
//         Cp = diag[x_pl^2, 1/x_pl, 1/x_pl]
// where x_pl = exp(2*eps_pl/3) and eps_pl is scalar plastic strain variable.
// Elastic tensor can be expressed as
//   b_el(V,eps_el) = (V/V0)^{2/3}*diag[x_el^2, 1/x_el, 1/x_el]
// and x_el = exp(2*eps_el/3), with eps_el scalar elastic strain variable
//   eps_el(V,eps_pl) = ln(V/V0) - eps_pl
// Note, with eps_pl = 0, elastic strain variable is
//   eps_el ~= V/V0 - 1, negative in compression !
//                       -----------------------
// Also note,
// Cauchy stress is linear combination of I, dev[b_el], dev[(b_el)^{-1}]
// and in 1-D
//   dev[b_el]/(V/V0)^{2/3} = (x_el^2-1/x_el^{-1})*diag[2, -1, -1]/3
// Hence dev[Cauchy stress] proportional to diag[2,-1,-1]
//       ------------------------------------------------ 
// Free energy in 1-D reduces to 
//    Psi(V,eps_el,T) = Psi_hydro(V,T) + Psi_shear(V,eps_el,T)
//    --------------------------------------------------------
// Note, free energy is function of V, T, eps_pl
//       since eps_el(V,eps_pl) = ln(V/V0)-eps_pl
// Hence (d/dV)|_{eps_pl} = (d/dV)|_{eps_el} + (1/V)*(d/d eps_el)|_V
// Cauchy stress tensor has form
//    sigma  = -(Phydro+Pshear)*I -(Pdev/2)*diag[2,-1,-1]
// where
//    Phydro = -(d/d V)Psi_hydro,            at constant T
//    Pshear = -(d/d V)Psi_shear,            at constant eps_el, T
//      Pdev = -(1/V)*(d/d eps_el)Psi_shear, at constant V, T
// Longitudinal component of stress is
//    sigma^{xx} = -(Phydro+Pshear+Pdev)
// Thermodynamic identity
//    d(e) = sigma^{xx} d(V) + T d(eta) + V*Pdev d(eps_pl)
//    ----------------------------------------------------
// EOS::P = -sigma^{xx}
// For ExtEOS, plastic strain is internal degree of freedom
// associated with relaxation of shear stress.
// Plastic-strain rate is constrained in order that
// changes in eps_pl are dissipative, that is, 
//   (d/dt)eps_pl has same sign as -Pdev (strain negative in compression)
//
class Shear1D : public EOSbase                  // abstract base class
{
private:
    Shear1D();                                  // disallowed
    Shear1D(const Shear1D&);                    // disallowed, use duplicate
    void operator=(const Shear1D&);             // disallowed
public:    
    Shear1D(const char *stype);
    Shear1D *Duplicate(){return static_cast<Shear1D*>(EOSbase::Duplicate());}
    int InitBase(Parameters &, Calc &, DataBase *db=NULL);
    virtual ~Shear1D();
    virtual int InitParams(Parameters &, Calc &, DataBase *db=NULL) = 0;
    virtual void PrintParams(std::ostream &out)                  = 0;
    virtual int ConvertParams(Convert &convert)             = 0;
public:    
    virtual double     F(double V, double T, double eps_el) = 0;
      // F is specific Helmholtz free energy
    virtual double     S(double V, double T, double eps_el) = 0;
      // S = - (d/dT) F is entropy
    virtual double     e(double V, double T, double eps_el) = 0;
      // e = F + T*S is specific internal energy
    virtual double    Ps(double V, double T, double eps_el) = 0;
      // Ps = -(d/dV) F is shear pressure
    virtual double  Pdev(double V, double T, double eps_el) = 0;
      // Pdev = -(1/V)(d/d eps_el) F is shear stress
    virtual double    CV(double V, double T, double eps_el) = 0;
      // CV = -T (d^2/d T^2) F = specific heat at constant V
      //    = CV_hyd + CV_shear
    virtual double dPdT(double V, double T, double eps_el)  = 0;
      //  dPdT = (d/dT)(Ps+Pdev)
      // Gamma = -(V/T)(dT/dV), const S
      //       = V*(d sigma^{xx}/d T)/CV, const V
      //         where sigma^{xx} = Phyd + Ps +Pdev, CV = CV_hyd+CV_shear
      //       = (CV_hyd*Gamma_hyd + V*dPdT_shear)/(CV_hyd+CV_shear)
    virtual double  dPdev_deps(double V, double T, double eps_el) = 0;
      // dPdev_deps = -(d/d eps_el) Pdev,   eps_el < 0 for Pdev > 0
      //            = (4/3)*G + O(eps_el)
    virtual double   cT2(double V, double T, double eps_el)       = 0;
      // cT2_s = -V*(V*d/dV+d/deps_el)(Ps+Pdev), isothermal wave speed squared
      // c2    = (cT2_hyd+cT2_s) + Gamma^2*T*CV, isentropic wave speed squared
      //       = V*(K+(4/3)*G)
    virtual double   cs2(double V, double T, double eps_el)       = 0;
      // cs2 = shear wave speed squared from acoustic tensor
      //     = G*V + O(eps_el)
};
//
inline int deleteShear1D(Shear1D *&shear)
{
    int status = deleteEOSbase(shear,"Shear1D");
    shear = NULL;
    return status;
}
//
// Database
// 
inline Shear1D *FetchShear1D(const char *type, const char *name, DataBase &db)
            { return (Shear1D *) db.FetchObj("Shear1D",type,name);}
inline Shear1D *FetchNewShear1D(const char *type, const char *name, DataBase &db)
            { return (Shear1D *) db.FetchNewObj("Shear1D",type,name);}
inline Shear1D *FetchShear1D(const char *type_name, DataBase &db)
            { return (Shear1D *) db.FetchObj("Shear1D",type_name);}
inline Shear1D *FetchNewShear1D(const char *type_name, DataBase &db)
            { return (Shear1D *) db.FetchNewObj("Shear1D",type_name);}
// print
inline ostream & operator<<(ostream &out, Shear1D &shear)
            { return shear.Print(out); }

#endif // EOSLIB_SHEAR1D_H
