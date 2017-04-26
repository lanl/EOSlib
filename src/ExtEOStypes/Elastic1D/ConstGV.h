#ifndef EOSLIB_CONST_GV_H
#define EOSLIB_CONST_GV_H

#include "Shear1D.h"
//
#define ConstGV_vers "ConstGVv2.0.1"
#define ConstGV_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *ConstGVlib_vers;
extern const char *ConstGVlib_date;
//
extern "C" { // DataBase functions
    char *Shear1D_ConstGV_Init();
    char *Shear1D_ConstGV_Info();
    void *Shear1D_ConstGV_Constructor();
}
//
// shear energy
// e(V,T,be) = 0.5*V*G*(I_1(be/J^{2/3})-3)
// ConstGV: e independent of T, and V*G = constant
// --------------------------------------
//    be = F*Cp^{-1}*F^T, elastic strain
//   I_1 = first invariant (trace)
// Reduces in 1-D to
// e(V,T,eps_el) = 0.5*V*G*(exp(4/3*eps_el)+2*exp(-2/3*eps_el)-3)
// 
class ConstGV : public Shear1D
{
private:
    ConstGV(const ConstGV&);                    // disallowed, use duplicate
    void operator=(const ConstGV&);             // disallowed
public:
    double GV;          // G(V) = GV/V
    ConstGV();
    ~ConstGV();
    ConstGV *Duplicate(){return static_cast<ConstGV*>(EOSbase::Duplicate());}
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(std::ostream &out);
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
 // Extra functionality for ConstGV
    double d2PdV2(double V, double eps_el);
      //  d2PdV2 = (d/dV+(1/V)*d/deps_el)^2 (Ps+Pdev)
      //         = -(d/dV+(1/V)*d/deps_el) (cT2/V^2)
      //  FD = ([rho*c^2*FD]_hydro + 0.5*V^2*ConstGV->d2PdV2)/(rho*c^2)
      //     = ([c^2*FD]_hydro + 0.5*V^3*ConstGV->d2PdV2)/(c_hydro^2+cT2)
    double          F(double V, double eps_el);
    double          e(double V, double eps_el);
    double       Pdev(double V, double eps_el);
    double dPdev_deps(double V, double eps_el);
    double        cT2(double V, double eps_el);
    double        cs2(double V, double eps_el);
};
inline double ConstGV::F(double V, double eps_el)
{
    return e(V,eps_el);
}
inline double ConstGV::e(double V, double eps_el)
{
    double x = exp((2./3.)*eps_el);
    return 0.5*GV*(x*x+2/x-3);          // = (2/3)*G(V)*V*eps_el^2 +O(eps_el^3)
}
inline double ConstGV::Pdev(double V, double eps_el)
{
    double x = exp((2./3.)*eps_el);
    return -(2./3.)*(GV/V)*(x*x-1/x);   // = (4/3)*G(V)*eps_el + O(eps_el^2)
}
inline double ConstGV::dPdev_deps(double V, double eps_el)
{
    double x = exp((2./3.)*eps_el);
    return (4./9.)*(GV/V)*(2.*x*x+1./x);  // = (4/3)*G(V) + O(eps_el)
}
inline double ConstGV::cT2(double V, double eps_el)
{
    double x = exp((2./3.)*eps_el);
    return (2./9.)*GV*(x*x +5./x);      // = (4/3)*G(V)*V + O(eps_el)
}
inline double ConstGV::cs2(double V, double eps_el)
{
    return GV*exp((4./3.)*eps_el);      // = G(V)*V + O(eps_el)
}
inline double ConstGV::d2PdV2(double V, double eps_el)
{ // d2PdV2 = -(d/dV+(1/V)*d/deps_el) (cT2/V^2)
    double x = exp((2./3.)*eps_el);
    return (4./27.)*GV/(V*V*V)*(x*x +20./x);
}    
#endif // EOSLIB_CONST_GV_H
