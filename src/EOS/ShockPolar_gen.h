#ifndef EOSLIB_SHOCK_POLAR_GEN_H
#define EOSLIB_SHOCK_POLAR_GEN_H

#include <OneDFunction.h>
#include "ShockPolar.h"

class ShockPolar_gen : public ShockPolar, protected OneDFunction
{
    enum ShockPolar_type
    {
        THETA=1, THETA_MAX=2, SONIC=3
    };
private:    
    ShockPolar_gen();                               // disallowed
    ShockPolar_gen(const ShockPolar_gen&);          // disallowed
    void operator=(const ShockPolar_gen&);          // disallowed
    ShockPolar_gen *operator &();                   // disallowed    
protected:
    double f(double p);                             // OneDFunction::f
    ShockPolar_type type;
public:
    ShockPolar_gen(EOS &e);
    ~ShockPolar_gen();
    int Initialize(PolarState &s);
    
    int ThetaLow(double theta, int dir, PolarWaveState &polar);
    int ThetaHi (double theta, int dir, PolarWaveState &polar);
};

#endif // EOSLIB_SHOCK_POLAR_GEN_H
