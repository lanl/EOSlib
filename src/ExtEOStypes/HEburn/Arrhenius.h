#ifndef EOSLIB_ARRHENIUS
#define EOSLIB_ARRHENIUS

#include <HErate.h>
//
#define Arrhenius_vers "Arrheniusv2.0.1"
#define Arrhenius_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *Arrhenius_lib_vers;
extern const char *Arrhenius_lib_date;
//
extern "C" { // DataBase functions
    char *HErate_Arrhenius_Init();
    char *HErate_Arrhenius_Info();
    void *HErate_Arrhenius_Constructor();
}
//
//  Arrhenius kinetics
//  lambda = reaction progress variable (mass fraction of products)
//           0 = reactants (unburnt)
//           1 = products  (burnt)
//
//  (d/dt)lambda = (1-lambda)^n *k*(T/T_m)^m *exp(-T_a/T)
//             n = reaction order (0 <= n <= 1)
//             k = rate multiplier (frequency factor)
//           T_a = activation temperature
//             m = exponent for power of T
//           T_m = reference temperature for algebraic dependence
//
class Arrhenius : public HErate
{
public:
    static double DLAMBDA;              // default for dlambda
    static double DLAMBDA_DT;           // default for dlambda_dt
    static double RMIN;                 // minimum rate for default T_cutoff
    static double TOL;                  // default for tol
private:
    Arrhenius(const Arrhenius &AR);     // disallowed
    Arrhenius *Duplicate();             // disallowed
    Arrhenius *operator &();            // disallowed
    void operator=(const Arrhenius&);   // disallowed        
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
    int step2(double &lambda, double dt);    // asymptotic for lambda ~ 1
public:
    double n;           // reaction order
    double k;           // rate multiplier
    double T_a;         // activation temperature
    double m;           // exponent for power of T
    double T_m;         // reference temperature for power of T
    double T_cutoff;    // Rate=0 if T < T_cutoff
    double T_max;       // max temperature to limit rate
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
    int count, min_level, max_level;    // diagnostic for Integrate()
    // add limit for TimeStep()
    Arrhenius();
    ~Arrhenius();
    IDOF *Idof();
    //
    double Rate(double T);
    double Rate(double lambda, double T);
    // z[] = {lambda}
    int Rate(double V, double e, const double *z, double *zdot);
    int TimeStep(double V, double e, const double *z, double &dt);
     // dt = 0 if no restriction (lambda=1 or T<T_cutoff)
    int Integrate(double V, double e, double *z, double dt);
    double Dt(double V, double e, const double *z, double lambda);
};

inline double Arrhenius::Rate(double T)
    { if(T>T_max) T=T_max; return k*pow(T/T_m,m)*exp(-T_a/T); }    
inline double Arrhenius::Rate(double lambda, double T)
    { return pow(1.-lambda,n)*Rate(T); }    

#endif // EOSLIB_ARRHENIUS
