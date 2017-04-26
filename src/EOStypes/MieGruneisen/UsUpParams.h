#ifndef EOSLIB_USUP_PARAMS_H
#define EOSLIB_USUP_PARAMS_H

#include <LocalMath.h>
#include <ODE.h>

// Reference curve is principal Hugoniot defined by
// us = c0 + s*up and up/us = 1 - V/V0
// for (1-1/s)V0 < V <= V0, compression branch up to maximum shock compression
//
// P(V) = P0 - K0*(V/V0 - 1) + G0*K0(V/V0 - 1)^2 for V > V0 and P > - Pspall
//  where K0 = rho_0*c_0^2 = bulk modulus
//        G0 = 2*s = Fundamental derivative
//  note P(V) monotonic up to V <= V0*(1+0.5/G0) and P >= P0 - 0.25*K0/G0
// e(V) = e0 - int^V_V0 P(V) dV,        e on isentrope
// T(V) = exp(int^V_V0 Gamma/V dV)*T0,  T on isentrope
//
// Extended in expansion upto cref(Vmax) = 0
// Note d(Pref)/dV = 0 at V = V0*(1+1/s) > Vmax
// At V0 Hugoniot and rearefaction curve continuous upto second derivative.
// 
class UsUpParams : protected ODE
{
private:
    void operator=(const UsUpParams&);        // disallowed
    UsUpParams(const UsUpParams&);            // disallowed
public:
// initial state
    double V0;
    double P0;
    double e0;
    double T0;
    double S0;
// us = c0 + s*up
    double c0;
    double s;
    double K0;                  // c0*c0/V0
    // double Vmin = (1-1/s)*V0;
// expansion region
    double Vmax;
    double Pmin;
// Gamma = G0 + G1*(V/V0 - 1)
    double G0;
    double G1;
    double Cv;                  // specific heat at constant V
// Mie-Gruneisen functions
    double Pref(double V);
    double dPref(double V);     // d(Pref)/d(V)
    double d2Pref(double V);    // d^2(Pref)/d(V)^2
    double eref(double V);
    double deref(double V);     // d(eref)/d(V)
    double d2eref(double V);    // d^2(eref)/d(V)^2
    double Tref(double V);
    double GoV(double V);       // Gamma/V
    double dGoV(double V);      // d(Gamma/V)/d(V)
    double d2GoV(double V);     // d^2(Gamma/V)/d(V)^2
    double intGoV(double V);    // int dV (Gamma/V)
    double c2ref(double V);
// Hugoniot reference curve
    double Us(double eta) const { return c0/(1-s*eta);}// eta = 1-V/V0 = up/us   
protected:
// ref = principal Hugoniot locus
// (d/dV)Tref + (Gamma/V)Tref = 0.5*[d(Pref)/dV*(V0-V) + Pref]/Cv
// Ref. Walsh & Christian, Phys. Rev. 97 (1955) pp. 1544-1556, Eq. (24)
    int F(double *yp, const double *y, double V); // ODE::F
    int copy(const UsUpParams &params);
    int init(EOSbase::Error *EOSerror);
public:    
    UsUpParams();
    ~UsUpParams();
    void InitParams(Calc &calc);
    void PrintParams(ostream &out) const;
    int ConvertParams(Convert &convert, EOSbase::Error *EOSerror);
    int ParamsOK(EOSbase::Error *EOSerror);
};

#endif // EOSLIB_USUP_PARAMS_H
