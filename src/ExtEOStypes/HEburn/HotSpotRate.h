#ifndef EOSLIB_HOTSPOT_RATE
#define EOSLIB_HOTSPOT_RATE

#include <HErate.h>
//
#define HotSpotRate_vers "HotSpotRate_v2.0.2"
#define HotSpotRate_date "May 18, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *HotSpotRate_lib_vers;
extern const char *HotSpotRate_lib_date;
//
extern "C" { // DataBase functions
    char *HErate_HotSpotRate_Init();
    char *HErate_HotSpotRate_Info();
    void *HErate_HotSpotRate_Constructor();
}
//
//  HotSpot reaction rate (SURF model)
//  Ref: Reactive burn models and Ignition & Growth concept
//       R. Menikoff and M. Sam Shaw
//       New Models Conference 2010
//       http://dx.doi.org/10.1051/epjconf/20101000003
//      
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
//      z[3] = s,  scaled reaction length   (dimensionless)
//
//  lambda(t,Ps)  = 0,                      for t<0
//                = g(s),                   for t>0
//  reaction rate = (d/dt)lambda
//                = (dg/ds)*(ds/dt)
//
//           g(s) is reaction scale function
//                   accounts for hot-spot growth geometry
//        (d/dt)s = 0,                      for t<0
//                = f(Ps)*D(p(t),Ps)/tref,  for t>0
//
//          f(Ps) is shock strength factor
//                   proportional to (hot spot density)^1/3
//        D(p,Ps) is deflagration speed factor
//           p(t) is pressure based on EOS and local particle state (V,e,lambda)
//                   (V,e) supplied by hydro code
//  reference quantities (Pref, tref) correspond to choice of units
//
//  specific choice of fitting forms
//         g(s)  = 1 - exp(-s^2)
//        f0(Ps) = exp( A + B*Ps/Pref )
//
//               = 0,                                    for      Ps <= P0
//         f(Ps) = f0(Ps) - f0(P0)*(1+B*(Ps-P0)/Pref),   for P0 < Ps <= P1
//               = f1*(1+df1*[1-exp(-B2*(Ps-P1)/Pref)]), for P1 < Ps
//         where
//            f1 = f(P1)
//            B2 = (Pref/f1)*(df/dPs)(P1) / abs(df1)
//               = [B/abs(df1)] * [f0(P1) - f0(P0)]/f1
//
//       D(p,Ps) = (p/Ps)^n
//  
//
class HotSpotRate : public HErate
{
public:
    static double DLAMBDA;              // default for dlambda
    static double TOL;                  // default for tol
private:
    HotSpotRate(const HotSpotRate &HS); // disallowed
    HotSpotRate *Duplicate();           // disallowed
    HotSpotRate *operator &();          // disallowed
    void operator=(const HotSpotRate&); // disallowed        
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
    double Pref;        // pressure unit
    double tref;        // time unit
    double n;           // scaling exponent for deflagration speed
    double A;           // dimensionless parameter for f(Ps), Ps<P1
    double B;           // dimensionless parameter for f(Ps), Ps<P1
    double P0;          // minimum pressure
    double P1;          // switch pressure
    double df1;         // f(Ps) -> f1*(1+df1) for Ps >> P1
    //
    double fp0;         // fp0 = f0(P0)
    double f1;          // f1  = f(P1)
    double B2;          // B2 such that df/dPs continuous at Ps=P1
    //
    // cutoff: g(s) = 1 for s >= s2
    //
    double s1;          // transition point for g(s)
    double s2;          // g(s) = 1 for s>s2
    double g1,dg1,d2g1; // g(s1), dg/ds(s1), d2gds2(g1)
    double gf;          // scale factor
    //
    HotSpotRate();
    ~HotSpotRate();
    IDOF *Idof();
    //
    // z[] = {Ps,ts,lambda,s}
    int Rate(double V, double e, const double *z, double *zdot);
    int TimeStep(double V, double e, const double *z, double &dt);
     // dt = 0 if no restriction (lambda=1 or Ps=0 or ts<=0)
    int Integrate(double V, double e, double *z, double dt);
    double Dt(double V, double e, const double *z, double lambda);
    //
    double D(double p);     // deflagration speed
    double g(double s);     // lambda = g(s)
    double dgds(double s);  // dg/ds
    double f(double ps);    // r0 = f(ps)
    double dfdp(double ps); // df/dp
};

inline double HotSpotRate::D(double p)
    { return (n==0.) ? 1. : pow(p, n); }  // p = P/Ps
inline double HotSpotRate::g(double s)
    { return (s<s1) ? gf*(1.-exp(-s*s)) :
                      ( (s<s2) ? gf*(g1+(s-s1)*(dg1+0.5*(s-s1)*d2g1)) : 1.);}
inline double HotSpotRate::dgds(double s)
    { return (s<s1) ? 2.*gf*s*exp(-s*s) : (s<s2 ? gf*(dg1+d2g1*(s-s1)): 0.); }

#endif // EOSLIB_HOTSPOT_RATE
