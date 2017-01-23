#ifndef EOSLIB_ISOTHERM_ODE_H
#define EOSLIB_ISOTHERM_ODE_H

#include <ODE.h>

class Isotherm_ODE : public Isotherm, public ODE
{
private:
    Isotherm_ODE();                         // disallowed
    Isotherm_ODE(const Isotherm_ODE&);      // disallowed
    void operator=(const Isotherm_ODE&);    // disallowed
    Isotherm_ODE *operator &();             // disallowed
public:
    Isotherm_ODE(EOS &e, double pvac = EOS::NaN);
    ~Isotherm_ODE();    
    int Initialize(const HydroState &state);
    int F(double *yp, const double *y, double V);

    int P(double p, ThermalState &state);
    int V(double v, ThermalState &state);   
};

#endif // EOSLIB_ISOTHERM_ODE_H
