#ifndef EOSLIB_FF_RATE
#define EOSLIB_FF_RATE

#include <HErate.h>
#include <ReactiveHugoniot.h>
//
#define FFrate_vers "FFratev2.0.1"
#define FFrate_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *FFrate_lib_vers;
extern const char *FFrate_lib_date;
//
extern "C" { // DataBase functions
    char *HErate_FFrate_Init();
    char *HErate_FFrate_Info();
    void *HErate_FFrate_Constructor();
}
//
// Rate = (1-lambda)^n * FFrate(P), with n <= 1
//
class FFrate : public HErate            // Forest Fire
{
public:
    static double DLAMBDA;              // default for dlambda
    static double DLAMBDA_DT;           // default for dlambda_dt
    static double TOL;                  // default for tol
private:
    FFrate(const FFrate &AR);           // disallowed
    FFrate *Duplicate();                // disallowed
    FFrate *operator &();               // disallowed
    void operator=(const FFrate&);      // disallowed        
protected:
    HErate *Copy(PTequilibrium *eos);   
    void PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
    int InitBurn(PTequilibrium *eos);
    void PrintParams(std::ostream &out);
    int ConvertParams(Convert &convert);
    // internal for Integrate()
    double V0,e0;
    double w1, w2;
    double rate1;
    int step1(double  lambda, double dt);   // step for dlambda
    int step2(double &lambda, double dt);   // asymptotic for lambda ~ 1
public:
    double dlambda;     // limit for Integrate(),             integrate=1
    double dlambda_dt;  // limit for TimeStep               source_type=3
    double tol;         // tolerance for Integrate(),         integrate=1
    double dt_min;      // minimum returned by TimeStep(),  source_type=3
    int source_type;    // return type for TimeStep()
                        //  1, dt = time_step for Rate
                        //  2, use (stiff) integrate to avoid small time step
                        //  3, Integrate() available
    int integrate;      // Flag for integrate method
                        //  0, rate=(1-lambda)*constant
                        //  1, integrate to tolerance, modified midpoint scheme
    // Pop plot parameters: log_10(P/Pref) = a - b*log_10(x/xref)
    double a;
    double b;
    double Pref, xref;
    ReactiveHugoniot hug;
    double n;           // reaction order
    double lambda_max;
    double Pmax;
    double Rmax;
    //
    int count, min_level, max_level;    // diagnostic for Integrate()
    //
    FFrate();
    ~FFrate();
    IDOF *Idof();
    //
    double Xpop(double p);
    double Rate(double P);
    double Rate(double lambda, double P);
    // z[] = {lambda}
    int Rate(double V, double e, const double *z, double *zdot);
    int TimeStep(double V, double e, const double *z, double &dt);
     // dt = 0 if no restriction (lambda=1)
    int Integrate(double V, double e, double *z, double dt);
    double Dt(double V, double e, const double *z, double lambda);
};

#endif // EOSLIB_FF_RATE
