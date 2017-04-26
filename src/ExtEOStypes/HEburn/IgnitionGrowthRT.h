#ifndef EOSLIB_IGNITION_GROWTH_RT
#define EOSLIB_IGNITION_GROWTH_RT

#include <IgnitionGrowth.h>
//
#define IgnitionGrowthRT_vers "IgnitionGrowthRT_v2.0.1"
#define IgnitionGrowthRT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *IgnitionGrowthRT_lib_vers;
extern const char *IgnitionGrowthRT_lib_date;
//
extern "C" { // DataBase functions
    char *HErate_IgnitionGrowthRT_Init();
    char *HErate_IgnitionGrowthRT_Info();
    void *HErate_IgnitionGrowthRT_Constructor();
}
//
//  IgnitionGrowthRT promotes IgnitionGrowth to enable tracking lead shock
//                that is, adds Ps, ts to IDOF which are ignored.
//  ---------------------
//  lambda = reaction progress variable (mass fraction of products)
//           0 = reactants (unburnt)
//           1 = products  (burnt)
//  HotSpotHE IDOF
//      z[0] = Ps = lead shock pressure     parameter supplied by hydro code
//      z[1] = t  = time - ts               parameter supplied by hydro code
//             time is that of hydro code
//               ts is shock arrival time
//      z[2] = lambda                       (dimensionless)
//
//
class IgnitionGrowthRT : public IgnitionGrowth
{
private:
    IgnitionGrowthRT(const IgnitionGrowthRT &HS); // disallowed
    IgnitionGrowthRT *Duplicate();             // disallowed
    IgnitionGrowthRT *operator &();            // disallowed
    void operator=(const IgnitionGrowthRT&);   // disallowed        
protected:
    HErate *Copy(PTequilibrium *eos);   
    void PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
    void PrintParams(std::ostream &out);
    int ConvertParams(Convert &convert);
public:
    //
    IgnitionGrowthRT();
    ~IgnitionGrowthRT();
    IDOF *Idof();
    //
    // z[] = {Ps,ts,lambda}
    int Rate(double V, double e, const double *z, double *zdot);
    int TimeStep(double V, double e, const double *z, double &dt);
     // dt = 0 if no restriction (lambda=1 or Ps=0 or ts<=0)
    int Integrate(double V, double e, double *z, double dt);
    double Dt(double V, double e, const double *z, double lambda);
};

#endif // EOSLIB_IGNITION_GROWTH_RT
