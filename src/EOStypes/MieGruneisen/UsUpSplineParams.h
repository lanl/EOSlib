#ifndef EOSLIB_USUP_SPLINE_PARAMS_H
#define EOSLIB_USUP_SPLINE_PARAMS_H

#include <LocalMath.h>
#include <ODE.h>
#include <Spline.h>
//
// Reference curve in compression is principal Hugoniot defined by
// Us^-1(eta) = spline Us_inv, where eta = Up/Us = 1-V/V_0
//   and asymptotically linear Us-Up for eta->eta_max=1/s1
// Requirements: us and up/us are monotonic
// Tref defined by ODE
//   (d/dV)Tref + (Gamma/V)Tref = 0.5*[d(Pref)/dV*(V0-V) + Pref]/Cv
//   Ref. Walsh & Christian, Phys. Rev. 97 (1955) pp. 1544-1556, Eq. (24)
//
// Reference curve in expanssion is Taylor expansion of isentrope
// P(V) = P0 + K0*(1- V/V0) + FD0*K0(1 - V/V0)^2 for V > V0
//  where K0 = rho_0*c_0^2 = bulk modulus
//        FD0 = 2*dUs/dUp|_0 = Fundamental derivative
//        dUs/dUp|_0 = -Us*d(Us_inv)/d(eta) at eta = 0
//  Vmax = V0*(1+0.5/FD0) and Pmin = P0 - 0.25*K0/FD0
//      corresponds to dP/dV = 0
// e(V) = e0 - int^V_V0 P(V) dV,        e on isentrope
// T(V) = exp(int^V_V0 Gamma/V dV)*T0,  T on isentrope
//
//  Hugoniot and isentrope are continuous upto second derivative at V0
//  
class UsUpSplineParams : protected ODE
{
private:
    void operator=(const UsUpSplineParams&);        // disallowed
    UsUpSplineParams(const UsUpSplineParams&);      // disallowed
protected:
    Spline Us_inv;      // ToDo: upgrade to smooth spline
public:
// initial state
    double V0;
    double P0;
    double e0;
    double T0;
    double S0;
// asymptotics: us -> c1 + s1*up, for V->V1 (up,us large)
    double s1;
    double V1;                  // V1 = (1-1/s1)*V0, or eta_max = 1/s1
  //double c1;                  // -s1/(d/d eta)Us_inv at eta=1/s1
// data for spline
    double *us_data;
    double *up_data;
    int n_data;
    double dusdup0;             // d(us)/d(up) at eta = 0
// Gamma = G0 + G1*(V/V0 - 1)
    double G0;
    double G1;
// specific heat at constant V    
    double Cv;
// expansion region
    double K0;                  // Bulk modulus at V0
    double FD0;                 // Fundamental derivative at V0
    double Vmax;
    double Pmin;
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
    double rhoc2ref(double V);  // (rho*c)^2 = -d(Pref)/dV 
                                //             + (Gamma/V)(Pref +d(eref)/dV)
    double drhoc2ref(double V); // d[(rho*c)^2]/d(V)
// Hugoniot reference curve
    double Us(double eta) const; // eta = 1-V/V0 = up/us    
    int UsInv(double eta, double uinv[3]) const;
    int Legendre(double eta, double &c, double &s) const;
        // s = d(us)/d(up) and c = us- s*up
protected:
    int F(double *yp, const double *y, double V); // ODE::F
    int copy(const UsUpSplineParams &params);
    int init(EOSbase::Error *EOSerror);
public:    
    UsUpSplineParams();
    ~UsUpSplineParams();
    void InitParams(Calc &calc);
    void PrintParams(ostream &out) const;
    int ConvertParams(Convert &convert, EOSbase::Error *EOSerror);
    int ParamsOK(Calc &calc, EOSbase::Error *EOSerror);
};

#endif // EOSLIB_USUP_SPLINE_PARAMS_H
