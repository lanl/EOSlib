#ifndef EOSLIB_INTSCV_ODE_H
#define EOSLIB_INTSCV_ODE_H

#include <ODE.h>
//
// Free energy = ec(V) - T*IntSCvT(sT) + theta(V)*IntSCv(sT)
//  ec(V) = energy on cold curve (T=0)
//  scaled temperature, sT = T/theta(V)
//    theta(V) = Debye temperature
//    Gamma(V) = - dln[theta(V)]/dln(V)
//  specific heat,  Cv(V,T) = SCv(sT)
//  e_thermal(V,T) = theta(V)*IntSCv(sT)
//                   IntSCv(sT)  = int_0^sT SCv(sT) d(sT)
//     entropy(sT) = IntSCvT(sT) = int_0^sT SCv(sT) d(sT)/sT
//
class IntSCv_ODE : protected ODE
{
private:
    void operator=(const IntSCv_ODE&);                  // disallowed
    IntSCv_ODE(const IntSCv_ODE&);                      // disallowed
    IntSCv_ODE *operator &();                           // disallowed    
protected:
    IntSCv_ODE(double eps_a = 1, double eps_r = 1e-6);
    int F(double *yp, const double *y, double sT);      // ODE::F
    //
    double eps_abs;
    double eps_rel;
    double MaxNorm(const double *y0, const double *y1,
                   const double*, const double*);       // ODE::MaxNorm
public:        
    virtual double SCv(double sT) = 0;
    int IntSCv(double sT, double &value);
    int InvIntSCv(double value, double &sT);    // inverse function to IntSCv
    int IntSCvT(double sT, double &value);
    int InitODE(double abs_err, double rel_err);
};

#endif // EOSLIB_INTSCV_ODE_H

