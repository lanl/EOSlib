#ifndef EOSLIB_PE_SPLINE_PARAMS_H
#define EOSLIB_PE_SPLINE_PARAMS_H

#include <LocalMath.h>
#include <ODE.h>
#include <Spline.h>
//
// Mie-Gruneisen EOS
// P(V,e) = Pref(V) + (Gamma/V)(e-eref(V))
//   where Pref(V) and eref(V) are spline functions
// T(V,e) = Tref(V) + (e-eref(V))/Cv
// Tref defined by ODE
//   (d/dV)Tref + (Gamma/V)Tref = [Pref + deref/dV]/Cv
//   (d/dV){exp[int(Gamma/V)dV]Tref} = {[Pref + deref/dV]exp[int(Gamma/V)dV]}/Cv
// S(V,T) = S0 + Cv*(log(T/T0)+intGoV(V));
//  
//
class PeSplineParams : protected ODE
{
private:
    void operator=(const PeSplineParams&);          // disallowed
    PeSplineParams(const PeSplineParams&);          // disallowed
protected:
    Espline P;      // P(eta) where eta = 1-V/V0
    Espline e;      // e(eta)
public:
// initial state
    double V0;
    double P0;
    double e0;
    double T0;
    double S0;
// data for spline: ToDo, replace with variable dimension arrays
    double   *P_data;
    double   *e_data;
    int       n_data;   // number of data points (knots)
    double Vmin;
    double dPdV0, dPdV1;
    double dedV0, dedV1;
// Gamma = G0 + G1*(V/V0 - 1) + G2*(V/V0 - 1)^2
    double G0;
    double G1;
    double G2;
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
protected:
    int F(double *yp, const double *y, double V);               // ODE::F
    //double MaxNorm(const double *y_full, const double *y_2half,
    //            const double *dy_full, const double *dy_2half); //ODE:MaxNorm
    int copy(const PeSplineParams &params);
    int init(EOSbase::Error *EOSerror);
    int Reinit(EOSbase::Error *EOSerror);
public:    
    PeSplineParams();
    ~PeSplineParams();
    void InitParams(Calc &calc);
    void PrintParams(ostream &out) const;
    int ConvertParams(Convert &convert, EOSbase::Error *EOSerror);
    int ParamsOK(Calc &calc, EOSbase::Error *EOSerror);
};

#endif // EOSLIB_PE_SPLINE_PARAMS_H
