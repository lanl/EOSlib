#ifndef EOSLIB_REACTIVE_HUGONIOT
#define EOSLIB_REACTIVE_HUGONIOT
//
#include <LocalMath.h>
#include <ODE.h>
#include "PTequilibrium.h"
//
class ReactiveHugoniot : public ODE
{
//  d(lambda)/d(u)
public: // internal variables
    double  y[1];
    double yp[1];
public:
    PTequilibrium *HE;
    EOS *Reactants();   // reactants = HE->eos1, HE->lambda1 = 1-lambda
    EOS *Products();    // products  = HE->eos2, HE->lambda2 = lambda
    double P(double V, double e, double lambda);
    double T(double V, double e, double lambda);
    double c(double V, double e, double lambda);    
    int set_lambda(double lambda);
    int ddlambda(double V, double e, double lambda, double &dP, double &dT);
    WaveState ws0;      // initial unburnt state (lambda=0)
    WaveState CJ;
    // default reactive hugoniot: us = c0 +s*u
    double c0;
    double s;
    virtual double Us(double u);
    virtual double dUsdUp(double u);
    //
    ReactiveHugoniot();
    ~ReactiveHugoniot();
    int Init(PTequilibrium *he, double sp=0.0);
    int F(double *y_prime, const double *y, double u); // ODE::F
    //
    WaveState ws;
    double ws_lambda;
    void Hstate(double u);   // set ws from Hugoniot jump conditions
    int Lambda(double &lambda);
    int P(double p);
    int U(double u);
private:
    class lambda_ODE;
    class P_ODE;
};
inline EOS *ReactiveHugoniot::Reactants()
    { return (HE && HE->eos1) ? HE->eos1->Duplicate() : NULL;}
inline EOS *ReactiveHugoniot::Products()
    { return (HE && HE->eos2) ? HE->eos2->Duplicate() : NULL;}

#endif // EOSLIB_REACTIVE_HUGONIOT
