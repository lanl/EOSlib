#ifndef EOSLIB_HOTSPOT_RATE2
#define EOSLIB_HOTSPOT_RATE2

#include <HErate.h>
//
#define HotSpotRate2_vers "HotSpotRate2_v2.0.1"
#define HotSpotRate2_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *HotSpotRate2_lib_vers;
extern const char *HotSpotRate2_lib_date;
//
extern "C" { // DataBase functions
    char *HErate_HotSpotRate2_Init();
    char *HErate_HotSpotRate2_Info();
    void *HErate_HotSpotRate2_Constructor();
}
//
//  HotSpot2 reaction rate (Gausian hot spot temperature distribution)
//  ---------------------
//  lambda = reaction progress variable (mass fraction of products)
//           0 = reactants (unburnt)
//           1 = products  (burnt)
//  HotSpotHE2 IDOF
//      z[0] = Ps = lead shock pressure     parameter supplied by hydro code
//      z[1] = Ts = lead shock termperature parameter supplied by hydro code
//      z[2] = t  = time - ts               parameter supplied by hydro code
//             time is that of hydro code
//               ts is shock arrival time
//      z[3] = lambda                       (dimensionless)
//      z[4] = s,  scaled reaction length   (dimensionless)
//      z[5] = s0, (d/dt)s at shock front   (dimensionless)
//
//  Rate = (d/dt) lambda = dg/ds * ds/dt
//
//  lambda(t,Ps,Ts) = 0,                      for t<0
//                  = g(s(t,Ps,Ts)),          for t>0
//  where     g(s)  = 1 - exp(-s^2),          simplest choice
//                    accounts for hot-spot growth geometry
//        s(t,Ps,Ts) = r(t)/rs(Ps,Ts),      dimensionless reaction length
//        r(t)    = int_0^t dt D(p(t)),     D(p) = D(Ps)*(p/Ps)^n
//                  distance reaction front propagates from single hot spot
//        p(t) is pressure based on EOS and local particle state (V,e,lambda)
//                  (V,e) supplied by hydro code
//        rs(Ps,Ts)  = [Nhs(Ps,Ts)]^(-1/3)
//          rs is length scale, choosen as average distance between hot spots
//          Nhs is active hot-spot number density
//  re-express s as
//  s = s0 * int_0^t (p/Ps)^n dt
//      where s0 = D(Ps)*[Nhs(Ps,Ts)]^(1/3) [units 1/time]
// (d/dt) s = s0 * (p/Ps)^n     for t >  0 (after  lead shock)
//          = 0                 for t <= 0 (before lead shock)
//
//   s0 = b * [erfc((Ti-Ths)/sigma)]^(1/3)
//   where Ths = Ts + a*Ps
//   model parameters
//     a        [temperature/pressure]
//     b        [1/time]
//     sigma    [temperature]
//     Ti       [temperature]
//     n        [dimensionless]
//
class HotSpotRate2 : public HErate
{
public:
    static double DLAMBDA;              // default for dlambda
    static double TOL;                  // default for tol
private:
    HotSpotRate2(const HotSpotRate2 &HS); // disallowed
    HotSpotRate2 *Duplicate();           // disallowed
    HotSpotRate2 *operator &();          // disallowed
    void operator=(const HotSpotRate2&); // disallowed        
protected:
    HErate *Copy(PTequilibrium *eos);   
    void PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
    void PrintParams(std::ostream &out);
    int ConvertParams(Convert &convert);
public:
    double lambda_burn; // complete burn if lambda > lambda_burn
    double dlambda;     // limit for TimeStep
    double tol;         // tolerance for Integrate()
    int source_type;    // return type for TimeStep()
                        //  1, dt = time_step for Rate
                        //  2, use (stiff) integrate to avoid small time step
                        //  3, Integrate() available, dt = time_step for Rate
    double smax;        // complete burn for s>smax, lambda_burn = g(smax)
    //
    double n;           // scaling exponent for deflagration speed
    double a;           // coefficient, d(Ths)/d(Ps)
    double b;           // max D(Ps)/rhs(Ps)
    double sigma;       // width of hot-spot distribution [temperature]
    double Ti;          // cutoff temperature for active hot spot
    //
    HotSpotRate2();
    ~HotSpotRate2();
    IDOF *Idof();
    //
    int Rate(double V, double e, const double *z, double *zdot);
    int TimeStep(double V, double e, const double *z, double &dt);
     // dt = 0 if no restriction (lambda=1 or Ps=0 or ts<=0)
    int Integrate(double V, double e, double *z, double dt);
    double Dt(double V, double e, const double *z, double lambda);
    //
    double D(double p);             // deflagration speed
    double g(double s);             // lambda = g(s)
    double dgds(double s);          // dg/ds
    double s0(double Ps,double Ts);
};

inline double HotSpotRate2::D(double p)
    { return pow(p, n); }  // p = P/Ps
inline double HotSpotRate2::g(double s)
    { return 1. - exp(-s*s); }
inline double HotSpotRate2::dgds(double s)
    { return 2.*s*exp(-s*s); }

#endif // EOSLIB_HOTSPOT_RATE2
