#ifndef EOSLIB_AV_ARRHENIUS_RATE
#define EOSLIB_AV_ARRHENIUS_RATE

#include <ExtEOS.h>
//
//  Arrhenius kinetics with averaged temperature
//  lambda = reaction progress variable (mass fraction of products)
//           0 = reactants (unburnt)
//           1 = products  (burnt)
//  Tav    = time averaged temperature
//
//  (d/dt)lambda = (1-lambda)^n *k*exp(-T_a/Tav)
//             n = reaction order (0 <= n <= 1)
//             k = rate multiplier (frequency factor)
//           T_a = activation temperature
//  (d/dt)Tav    = (T-Tav)/tau
//           T   = material temperature
//           tau = time constant
//    Tav(t) = Tav(0)*exp(-t/tau) + \int_0^t (dt'/tau)*T(t')*exp(-(t-t')/tau)
//
class AvArrheniusRate
{
protected:
    ExtEOS *eos;
    // internal for Integrate()
    double V0,e0;
    double z0[2],z1[2],z2[2],z3[2];
    double ddt0[2], ddt2[2];    
    int step0(double *z, double dt);
    int step1(double *z, double dt);
    int step2(double *z, double dt);
public:
    double n;           // reaction order
    double k;           // rate multiplier
    double T_a;         // activation temperature
    double T_cutoff;    // Rate=0 if Tav < T_cutoff
    double tau;         // time constant
    double dlambda;     // limit for Integrate()
    // add limit for TimeStep()

    AvArrheniusRate(ExtEOS &Eos);
    AvArrheniusRate(const AvArrheniusRate &AAR);
    ~AvArrheniusRate();
    void InitParams(Calc &calc);
    void PrintParams(ostream &out) const;
    int ConvertParams(Convert &convert);
    int ParamsOK();
    //
    double Rate(double lambda, double T);
    // z[] = {lambda,Tav}
    int Rate(double V, double e, const double *z, double *zdot);
    int TimeStep(double V, double e, const double *z, double &dt);
     // dt >= tau or dt = 0 if no restriction (lambda=1 or Tav<T_cutoff)
    int Integrate(double V, double e, double *z, double dt);
    double Dt(double V, double e, const double *z, double lambda);
};
inline double AvArrheniusRate::Rate(double lambda, double T)
    { return pow(1.-lambda,n)*k*exp(-T_a/T); }    

#endif // EOSLIB_AV_ARRHENIUS_RATE
