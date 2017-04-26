#ifndef EOSLIB_POLY_G_H
#define EOSLIB_POLY_G_H

#include "Polynomial.h"
#include "Shear1D.h"
//
#define PolyG_vers "PolyGv2.0.1"
#define PolyG_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *PolyGlib_vers;
extern const char *PolyGlib_date;
//
extern "C" { // DataBase functions
    char *Shear1D_PolyG_Init();
    char *Shear1D_PolyG_Info();
    void *Shear1D_PolyG_Constructor();
}
//
// shear energy
// e(V,T,be) = 0.5*V*G*(I_1(be/J^{2/3})-3)
// PolyG: e independent of T, and G = polynomial(eta)
//        where eta = 1 - V/V0
// --------------------------------------
//    be = F*Cp^{-1}*F^T, elastic strain
//   I_1 = first invariant (trace)
// Reduces in 1-D to
// e(V,T,eps_el) = 0.5*V*G(eta)*(exp(4/3*eps_el)+2*exp(-2/3*eps_el)-3)
// 
class PolyG : public Shear1D
{
private:
    PolyG(const PolyG&);                        // disallowed, use duplicate
    void operator=(const PolyG&);               // disallowed
public:
    double V0;
    Polynomial G; // G(eta) where eta = 1-V/V0;
    PolyG();
    ~PolyG();
    PolyG *Duplicate(){return static_cast<PolyG*>(EOSbase::Duplicate());}
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
public:    
    double     F(double V, double T, double eps_el);
      // F is specific Helmholtz free energy
    double     S(double V, double T, double eps_el);
      // S = - (d/dT) F is entropy
    double     e(double V, double T, double eps_el);
      // e = F + T*S is specific internal energy
    double    Ps(double V, double T, double eps_el);
      // Ps = -(d/dV) F is shear pressure
    double  Pdev(double V, double T, double eps_el);
      // Pdev = -(1/V)(d/d eps_el) F is shear stress
    virtual double  dPdev_deps(double V, double T, double eps_el);
      // dPdev_deps = -(d/d eps_el) Pdev,   eps_el < 0 for Pdev > 0
    double    CV(double V, double T, double eps_el);
      // CV = -T (d^2/d T^2) F = specific heat at constant V
      //    = CV_hyd + CV_shear
    double dPdT(double V, double T, double eps_el);
      //  dPdT = (d/dT)(Ps+Pdev)
    double   cT2(double V, double T, double eps_el);
      // cT2_s = -V*(V*d/dV+d/deps_el)(Ps+Pdev), isothermal wave speed squared
    double   cs2(double V, double T, double eps_el);
      // cs2 = shear wave speed squared from acoustic tensor
 // Extra functionality for PolyG
    double          F(double V, double eps_el);
    double          e(double V, double eps_el);
    double         Ps(double V, double eps_el);
    double       Pdev(double V, double eps_el);
    double dPdev_deps(double V, double eps_el);
    double        cT2(double V, double eps_el);
    double        cs2(double V, double eps_el);
    double     d2PdV2(double V, double eps_el);
      //  d2PdV2 = (d/dV+(1/V)*d/deps_el)^2 (Ps+Pdev)
      //         = -(d/dV+(1/V)*d/deps_el) (cT2/V^2)
      //  FD = ([rho*c^2*FD]_hydro + 0.5*V^2*PolyG->d2PdV2)/(rho*c^2)
      //     = ([c^2*FD]_hydro + 0.5*V^3*PolyG->d2PdV2)/(c_hydro^2+cT2)
};
inline double PolyG::F(double V, double eps_el)
{
    return e(V,eps_el);
}
inline double PolyG::e(double V, double eps_el)
{
    double x = exp((2./3.)*eps_el);
    double eta = 1.-V/V0;
    return 0.5*V*G.f(eta)*(x*x+2./x-3.);
}
inline double PolyG::Ps(double V, double eps_el)
{
    double x = exp((2./3.)*eps_el);
    double eta = 1.-V/V0;
    return -0.5*(G.f(eta)-(V/V0)*G.df(eta))*(x*x+2./x-3.);
}
inline double PolyG::Pdev(double V, double eps_el)
{
    double x = exp((2./3.)*eps_el);
    double eta = 1.-V/V0;
    return -(2./3.)*G.f(eta)*(x*x-1./x);
}
inline double PolyG::dPdev_deps(double V, double eps_el)
{
    double x = exp((2./3.)*eps_el);
    double eta = 1.-V/V0;
    return (4./9.)*G.f(eta)*(2.*x*x+1./x);
}
inline double PolyG::cT2(double V, double eps_el)
{
    double x = exp((2./3.)*eps_el);
    double eta = 1.-V/V0;
    return V*( (2./9.)*(7.*x*x -1./x)*G.f(eta)
           -((7./3.)*x*x +(2./3.)/x-3.)*(V/V0)*G.df(eta)
           +0.5*(x*x +2./x-3.)*sqr(V/V0)*G.d2f(eta) );
}
inline double PolyG::cs2(double V, double eps_el)
{
    double eta = 1.-V/V0;
    return V*G.f(eta)*exp((4./3.)*eps_el);
}
#endif // EOSLIB_POLY_G_H
