#ifndef EOSLIB_PRANDTL_MEYER_ODE_H
#define EOSLIB_PRANDTL_MEYER_ODE_H

#include "ShockPolar.h"
#include <ODE.h>

class PrandtlMeyer_ODE : public PrandtlMeyer, protected ODE
{
private:
    PrandtlMeyer_ODE();                             // disallowed
    void operator=(const PrandtlMeyer_ODE&);        // disallowed
    PrandtlMeyer_ODE(const PrandtlMeyer_ODE&);      // disallowed
    PrandtlMeyer_ODE *operator &();                 // disallowed
protected:
    int F(double *yp, const double *y, double V);   // ODE::F
public:
    PrandtlMeyer_ODE(EOS &e);
    ~PrandtlMeyer_ODE();
    int Initialize(PolarState &s);
    
    int Theta(double theta, int dir, PolarWaveState &polar);
    int P(double p, int dir, PolarWaveState &polar);
};

#endif //  EOSLIB_PRANDTL_MEYER_ODE_H
