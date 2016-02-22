#ifndef EOSLIB_P_N_RATE
#define EOSLIB_P_N_RATE

#include <HErate.h>
//
#define PnRate_vers "PnRate_v2.0.1"
#define PnRate_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *PnRate_lib_vers;
extern const char *PnRate_lib_date;
//
extern "C" { // DataBase functions
    char *HErate_PnRate_Init();
    char *HErate_PnRate_Info();
    void *HErate_PnRate_Constructor();
}
//
//  P^N reaction rate
//  lambda = reaction progress variable (mass fraction of products)
//           0 = reactants (unburnt)
//           1 = products  (burnt)
//
//  (d/dt)lambda = (1-lambda)^nu *k*(P/Pcj)**N
//            nu = reaction order (0 <= nu <= 1)
//             k = rate multiplier (frequency factor)
//             N = pressure exponent
//
class PnRate : public HErate
{
public:
    static double DLAMBDA;              // default for dlambda
    static double DLAMBDA_DT;           // default for dlambda_dt
    static double RMIN;                 // minimum rate for default P_cutoff
    static double TOL;                  // default for tol
private:
    PnRate(const PnRate &Pn);           // disallowed
    PnRate *Duplicate();                // disallowed
    PnRate *operator &();               // disallowed
    void operator=(const PnRate&);      // disallowed        
protected:
    HErate *Copy(PTequilibrium *eos);   
    void PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
    void PrintParams(std::ostream &out);
    int ConvertParams(Convert &convert);
    // internal for Integrate()
    double V0,e0;
    double y1, y2;
    double rate1;
    int step1(double lambda, double dt);    // step for dlambda
    int step2(double &lambda, double dt);   // asymptotic for lambda ~ 1
public:
    double nu;          // reaction order
    double k;           // rate multiplier
    double Pcj;         // pressure scale
    double N;           // pressure exponent
    double P_cutoff;    // Rate=0 if P < P_cutoff
    double lambda_burn; // complete burn if lambda > lambda_burn
    double dlambda;     // limit for Integrate()
    double dlambda_dt;  // limit for TimeStep
    double tol;         // tolerance for Integrate()
    double dt_min;      // minimum returned by TimeStep(),  source_type=3
    int source_type;    // return type for TimeStep()
                        //  1, dt = time_step for Rate
                        //  2, use (stiff) integrate (no time step constraint)
                        //  3, Integrate() available (time step constraint)
    int integrate;      // Flag for integrate method
                        //  0, rate=(1-lambda)*constant
                        //  1, integrate to tolerance, modified midpoint scheme
    int count, min_level, max_level;    // diagnostic for Integrate()
    // add limit for TimeStep()
    PnRate();
    ~PnRate();
    IDOF *Idof();
    //
    double Rate(double lambda, double P);
    // z[] = {lambda}
    int Rate(double V, double e, const double *z, double *zdot);
    int TimeStep(double V, double e, const double *z, double &dt);
     // dt = 0 if no restriction (lambda=1 or P<P_cutoff)
    int Integrate(double V, double e, double *z, double dt);
    double Dt(double V, double e, const double *z, double lambda);
};

inline double PnRate::Rate(double lambda, double P)
    { return pow(1.-lambda,nu)*k*pow(P/Pcj,N); }    

#endif // EOSLIB_P_N_RATE
