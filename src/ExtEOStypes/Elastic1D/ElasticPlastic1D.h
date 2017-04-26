#ifndef EOSLIB_ELASTIC_PLASTIC_1D_H
#define EOSLIB_ELASTIC_PLASTIC_1D_H

#include "Elastic1D.h"
#include <ExtEOS.h>
//
#define ElasticPlastic1D_vers "ElasticPlastic1Dv2.0.1"
#define ElasticPlastic1D_date "Jan. 5, 2013"
//
extern const char *ElasticPlastic1D_lib_vers;
extern const char *ElasticPlastic1D_lib_date;
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
// For ExtEOS, plastic strain is internal degree of freedom (IDOF)
// associated with relaxation of shear stress.
// Plastic-strain rate is constrained in order that
// changes in eps_pl are dissipative, that is, 
//   (d/dt)eps_pl has same sign as -Pdev (strain negative in compression)
//
class ElasticPlastic1D : public ExtEOS          // abstract base class
{
private:
    ElasticPlastic1D();                         // disallowed
    void operator=(const ElasticPlastic1D&);    // disallowed
    ElasticPlastic1D *operator &();             // disallowed, use Duplicate()
public:
    Elastic1D *elastic;
    //IDOF      *z_ref;     // ExtEOS,  must be set by derived class
    //int       frozen;     // ExtEOS
protected:
    ElasticPlastic1D(const char *ptype);
    ElasticPlastic1D(const ElasticPlastic1D&);
    virtual void PrintParams(ostream &out);
    virtual int ConvertParams(Convert &convert);    
    virtual int  PreInit(Calc &);
    virtual int PostInit(Calc &, DataBase *);
    //int ExtEOS::n;          // = z_ref::n
    //double *ExtEOS::ztmp;   // temporary storage for IDOF
public:
    virtual ostream &PrintComponents(ostream &out);
    virtual ~ElasticPlastic1D();
    ElasticPlastic1D *Duplicate();                  // ref_count
    //virtual ExtEOS *Copy();                       // Full copy (constructor)
 // ExtEOS functions
    // double   *z_f(double V, double e);
    // functions of (V,e)
    using ExtEOS::P;
    using ExtEOS::T;
    using ExtEOS::S;
    using ExtEOS::c2;
    using ExtEOS::Gamma;
    using ExtEOS::CV;
    using ExtEOS::CP;
    using ExtEOS::KT;
    using ExtEOS::beta;
    using ExtEOS::FD;    
    virtual double      P(double V, double e, const double *z);
    virtual double Phydro(double V, double e, const double *z);
    virtual double Pshear(double V, double e, const double *z);
    virtual double   Pdev(double V, double e, const double *z);
    virtual double      T(double V, double e, const double *z);
    virtual double      S(double V, double e, const double *z);
    virtual double     c2(double V, double e, const double *z);
    virtual double  Gamma(double V, double e, const double *z);
    virtual double     CV(double V, double e, const double *z);
    virtual double     CP(double V, double e, const double *z);
    virtual double     KT(double V, double e, const double *z);
    virtual double   beta(double V, double e, const double *z);
    virtual double     FD(double V, double e, const double *z);
    virtual int NotInDomain(double V, double e, const double *z);
    virtual int   Rate(double V, double e, const double *z, double *zdot) = 0;
    virtual int TimeStep(double V, double e, const double *z, double &dt) = 0;
    // int  Integrate(double V, double e, double *z, double dt);
    // int  Equilibrate(double V, double e, double *z);
 // plastic flow when yield_function > yields_strength
    double Eps_el(double V, double e);                  // elastic shear strain 
    double Eps_el(double V, double e, const double *z); // elastic shear strain    
    virtual double yield_function(double V, double e, const double *z) = 0;
    virtual double yield_strength(double V, double e, const double *z) = 0;
    //
    int var(const char *name, double V, double e, const double *z,
                double &value);   
};
//
inline ElasticPlastic1D *ElasticPlastic1D::Duplicate()
    { return static_cast<ElasticPlastic1D*>(EOSbase::Duplicate());  }  
inline double ElasticPlastic1D::Eps_el(double V, double e)
    { return Eps_el(V,e,z_f(V,e));                                  }
inline double ElasticPlastic1D::Eps_el(double V, double e, const double *z)
    { return elastic && z ? elastic->Eps_el(V) - z[0] : NaN;        }
 
#endif // EOSLIB_ELASTIC_PLASTIC_1D_H
