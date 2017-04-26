#ifndef EOSLIB_BIRCHMURNAGHAN_PARAMS_H
#define EOSLIB_BIRCHMURNAGHAN_PARAMS_H

#include "IntSCv_ODE.h"
#include "GenHayesFunc.h"
#include <EOSbase.h>

// ToDo: Replace ScV(sT) with spline
class BirchMurnaghanParams
{
public:
    double V0;                              // reference Specific Volume
    double e0;                              // reference e = e(V0,T=0)
    // cold curve parameters
    double K0;                              // Isothermal bulk modulus at V0
    double Kp0;                             // dK/dP at V0
    // Gamma = a + b*(V/V0)
    // Debye temperature
    // theta(V) = theta0 * (V0/V)^a * exp[b*(1-V/V0)]
    double a;
    double b;
    double theta0;
    // Specific heat parameters
    // Cv(V,T) = SCv(sT) where sT = T/theta(V)
    // SCv(sT) = Cv * sT^3/(cv[3]*sT^3+cv[2]*sT^2+cv[1]*sT+cv[0])
    double Cv, cv[4];
    double eps_abs, eps_rel;
    //
    // Extend cold curve into expansion (V>V0)
    // Note: Pmin < 0, i.e. allow expansion into tension regime
    //       but constrain -dPc/dV >= 0 by setting dPc=0 for V > Vmax
    double Vmax;    //    dPc(Vmax) = 0
    double Pmin;    //     Pc(Vmax)
    double e1;      //     ec(Vmax)
    //
    // Extend Gamma into expansion regime (V>V0)
    // Gamma(V)/V = a/V + (b/V0) * eps1/sqrt[(V/V0-1)^2+eps1^2]
    // -ln(theta(V)/theta0) = a*ln(V/V0)
    //                      + b*eps1*ln[(V/V0-1 + sqrt[(V/V0-1)^2+eps1^2])/eps1]
    // Note: Gamma/V and d(Gamma/V)/dV continuous at V0
    //       Hence sound speed is continuous
    //       Moreover d(Gamma/V)/dV < 0
    double eps1;    //  Gamma -> a + eps1*b as V -> infty
    //
    BirchMurnaghanParams();
    void InitParams(Calc &calc);
    void PrintParams(ostream &out) const;
    int ConvertParams(Convert &convert);
    int ParamsOK();
};


class BirchMurnaghanBase: public BirchMurnaghanParams, public IntSCv_ODE,
                          public virtual GenHayesFunc
{
protected:
    EOSbase *eos;
    BirchMurnaghanBase() : eos(NULL) {};
    BirchMurnaghanBase(const BirchMurnaghanBase &base);
public:
    double Pc(double V);                    // cold curve P
    double dPc(double V);                   // dPc/dV
    double ec(double V);                    // ec = - Int^V_V0 dV Pc + e0
    double theta(double V);                 // "Debye temperature"
    double Dln_theta(double V);             // - d ln(theta)/d V = Gamma/V
    double D2theta(double V);               // [d^2(theta)/d V^2] / theta
    double SCv(double sT);                  // Specific_heat(T/theta)
    double IntSCv(double sT);               // Int^sT_0 dT SCv(T)
    double InvIntSCv(double C);             // Inverse of IntCv
    double IntSCvT(double sT);              // Int^sT_0 dT/T SCv(T)
};

#endif // EOSLIB_BIRCHMURNAGHAN_PARAMS_H
