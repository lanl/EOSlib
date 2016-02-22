#ifndef EOSLIB_HAYESBM_PARAMS_H
#define EOSLIB_HAYESBM_PARAMS_H
#include <cmath>

class HayesBMparams
{
public:
    double V0, e0, P0, T0;              // reference state
    double K0, Kp0;                     // isothermal bulk modulus parameters
    double G0, G1;                      // Gruneisen parameters
    double Cv;                          // specific heat at constant V
    
    HayesBMparams();
    void InitParams(Calc &calc);
    void PrintParams(ostream &out) const;
    int ConvertParams(Convert &convert);
    int ParamsOK() const;

    double   P(double V) const; // P(V,T0)
    double  dP(double V) const; // dP/dV
    double d2P(double V) const; // (d/dV)^2 P
    double   e(double V) const; // - int_V0^V P(V) dV
    double eT0(double V) const  // e(V,T0)
        { return e(V) - Cv*T0*Gp(V); }
    double Gamma(double V) const  { return G0 + (G1/V0)*(V-V0); }
    double  Gp(double V) const  // Gp = - int_V0^V Gamma/V dV
        { return -(G1/V0)*(V-V0) - (G0-G1)*log(V/V0); }    
    double  dG(double V) const  // dG = d(Gamma/V)/dV
        { return -(G0-G1)/(V*V); }    
    double d2G(double V) const  // dG = (d/dV)^2 (Gamma/V)
        { return 2*(G0-G1)/(V*V*V); }    
};

#endif // EOSLIB_HAYESBM_PARAMS_H
