#ifndef EOSLIB_IGNITION_GROWTH
#define EOSLIB_IGNITION_GROWTH

#include <HErate.h>
//
#define IgnitionGrowth_vers "IgnitionGrowth_v2.0.1"
#define IgnitionGrowth_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *IgnitionGrowth_lib_vers;
extern const char *IgnitionGrowth_lib_date;
//
extern "C" { // DataBase functions
    char *HErate_IgnitionGrowth_Init();
    char *HErate_IgnitionGrowth_Info();
    void *HErate_IgnitionGrowth_Constructor();
}
//
//  Ignition & Growth burn model
//  Ref. E. L. Lee and C. M. Tarver
//       Phys. Fluids 23 (1980) 2362
//       Phenomenological model of shock initiation in heterogeneous explosives
//  lambda = reaction progress variable (mass fraction of products)
//           0 = reactants (unburnt)
//           1 = products  (burnt)
//
//  (d/dt)lambda = I*(1-lambda)^b*(rho/rho_0-1-a)^x, for lambda < lambda_igmax
//           + G1*(1-lambda)^c * lambda^d * P^y,     for lambda < lambda_G1max
//           + G2*(1-lambda)^e * lambda^g * P^z,     for lambda_G2min < lambda
//
class IgnitionGrowth : public HErate
{
public:
    static double DLAMBDA;                      // default for dlambda
    static double DLAMBDA_DT;                   // default for dlambda_dt
    static double TOL;                          // default for tol
private:
    IgnitionGrowth *Duplicate();                // disallowed
    IgnitionGrowth *operator &();               // disallowed
protected:
    IgnitionGrowth(const IgnitionGrowth &);
    void operator=(const IgnitionGrowth&);
    HErate *Copy(PTequilibrium *eos);   
    void PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
    void PrintParams(std::ostream &out);
    int ConvertParams(Convert &convert);
    // internal for Integrate()
    double V0,e0;
    double w1, w2;
    double rate1;
    int step1(double lambda, double dt);    // step for dlambda
    int step2(double &lambda, double dt);    // asymptotic for lambda ~ 1
public:
    double dlambda;                     // limit for Integrate()
    double dlambda_dt;                  // limit for TimeStep
    double tol;                         // tolerance for Integrate()
    //
    double rho0;
    double I;
    double a;
    double b;
    double x;
    double lambda_igmax;
    double G1;
    double c;
    double d;
    double y;
    double lambda_G1max;
    double G2;
    double e;
    double g;
    double zp;
    double lambda_G2min;
    //
    int count, min_level, max_level;    // diagnostic for Integrate()
    // add limit for TimeStep()
    IgnitionGrowth();
    ~IgnitionGrowth();
    IDOF *Idof();
    //
    double Rate(double lambda, double V, double p);
    // z[] = {lambda}
    int Rate(double V, double e, const double *z, double *zdot);
    int TimeStep(double V, double e, const double *z, double &dt);
     // dt = 0 if no restriction (lambda=1 or P<P_cutoff)
    int Integrate(double V, double e, double *z, double dt);
    double Dt(double V, double e, const double *z, double lambda);
};

#endif // EOSLIB_IGNITION_GROWTH
