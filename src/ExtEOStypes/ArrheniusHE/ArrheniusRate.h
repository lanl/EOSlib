#ifndef EOSLIB_ARRHENIUS_RATE
#define EOSLIB_ARRHENIUS_RATE

#include <ExtEOS.h>
//
//  Arrhenius kinetics
//  lambda = reaction progress variable (mass fraction of products)
//           0 = reactants (unburnt)
//           1 = products  (burnt)
//
//  (d/dt)lambda = (1-lambda)^n *k*exp(-T_a/Tav)
//             n = reaction order (0 <= n <= 1)
//             k = rate multiplier (frequency factor)
//           T_a = activation temperature
//
class ArrheniusRate
{
protected:
    ExtEOS *eos;
    // internal for Integrate()
    double V0,e0;
    double z0[1],z1[1],z2[1],z3[1];
    double ddt0, ddt2;    
    int step1(double *z, double dt);
    int step2(double *z, double dt);
public:
    double n;           // reaction order
    double k;           // rate multiplier
    double T_a;         // activation temperature
    double T_cutoff;    // Rate=0 if Tav < T_cutoff
    double dlambda;     // limit for Integrate()
    // add limit for TimeStep()

    ArrheniusRate(ExtEOS &Eos);
    ArrheniusRate(const ArrheniusRate &AR);
    ~ArrheniusRate();
    void InitParams(Calc &calc);
    void PrintParams(std::ostream &out) const;
    int ConvertParams(Convert &convert);
    int ParamsOK();
    //
    double Rate(double lambda, double T);
    // z[] = {lambda}
    int Rate(double V, double e, const double *z, double *zdot);
    int TimeStep(double V, double e, const double *z, double &dt);
     // dt = 0 if no restriction (lambda=1 or T<T_cutoff)
    int Integrate(double V, double e, double *z, double dt);
    double Dt(double V, double e, const double *z, double lambda);
};
inline double ArrheniusRate::Rate(double lambda, double T)
    { return pow(1.-lambda,n)*k*exp(-T_a/T); }    

#endif // EOSLIB_ARRHENIUS_RATE
