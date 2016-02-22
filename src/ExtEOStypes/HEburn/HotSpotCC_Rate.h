#ifndef EOSLIB_HOTSPOT_CC_RATE
#define EOSLIB_HOTSPOT_CC_RATE

#include <HErate.h>
//
#define HotSpotCC_Rate_vers "HotSpotCC_Rate_v2.0.1"
#define HotSpotCC_Rate_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *HotSpotCC_Rate_lib_vers;
extern const char *HotSpotCC_Rate_lib_date;
//
extern "C" { // DataBase functions
    char *HErate_HotSpotCC_Rate_Init();
    char *HErate_HotSpotCC_Rate_Info();
    void *HErate_HotSpotCC_Rate_Constructor();
}
//
//  Hot spot + carbon cluster reaction rates (SURF-plus model)
//  ----------------------------------------
//  lambda  = hot spot reaction progress variable
//           0 = reactants (unburnt)
//           1 = products  (burnt)
//  lambda2 = carbon cluster reaction progress variable
//           0 = no carbon clusters
//           1 = equilibrium carbon clusters
//  HotSpotCC_HE IDOF
//      z[0] = Ps = lead shock pressure     parameter supplied by hydro code
//      z[1] = t  = time - ts               parameter supplied by hydro code
//             time is that of hydro code
//               ts is shock arrival time
//      z[2] = lambda                        (dimensionless)
//      z[3] = s is scaled reaction length   (dimensionless)
//      z[4] = s2 = lambda2^(1/2)            (dimensionless)
//             lambda2 = carbon cluster reaction progress variable
//      z[5] = e_cc,     carbon cluster specific energy
//
//  Rate_s  = (d/dt) s
//          = f(Ps)*[p(t)/Ps]^n,                for t > 0
//  Rate_s2 = (d/dt) s2
//          = lambda1^2 * h'(h^{-1}(lambda2)),  for t > 0
//
//  lambda(s)   = g(s)
//  lambda2(s2) = s2^2
//
//  where
//    g(s)  is model function that accounts for hot-spot growth geometry
//    f(Ps) is model function proportional to hot-spot density
//     p(t) is pressure based on EOS and local state from hydro code,
//          = P(V, e-lambda1*de(lambda2), lambda1)
//  P(V,e,lambda) is P-T equilibium of reactants and equilibrium products
//    de(lambda2) is model function for carbon cluster energy
//     h(t) is model function for limiting cluster time dependence
//     h'(t) is (d/dt)h
//     h^{-1} is inverse function of lambda = h(t), i.e. t = h^{-1}(lambda)
//
// functions for hot spot model
//   g0(s) = 1 - exp(-s^2)
//
//   g(s) = gn^(-1)*g0(s),                                         for s < sc
//        = gn^{-1}*[g0(sc)+g0'(sc)*(s-sc)+0.5*g0''(sc)*(s-sc)^2], for sc < s
//          where sc is cutoff parameter
//            such that g(smax) = 1 for smax = sc - g0'(sc)/g0''(sc)
//            and gn = g0(sc) - 0.5*[g0'(sc)]^2 / g0''(sc)
//
//              = exp(A+B*Ps/Pref),                          for       Ps < Pign
//  t_ref*f(Ps) = [(P-Pign)*fmax+(Pmax-P)*fign]/(Pmax-Pign), for Pign < Ps <Pmax
//              = fmax,                                      for Pmax < Ps   
//   (Pref, tref) correspond to choice of units
//   ToDo: continuous derivative df/dPs at Ps = Pign and Ps = Pmax ?
//
// functions for carbon cluster model
//   e_cc = lambda*de(lambda2),  carbon cluster energy relative to equilibrium
//   Mixture EOS
//     P(V,e,lambda,lambda2) = HE->P(V,e-e_cc,lambda)
//     HE->P is PT equilibrium of reactants and equilibrium products (wrt CC)
//     or energy release in hydro code reaction step
//        (d/dt)e = -(d/dt) e_cc
//                = -[de(lambda2)*Rate_lambda + lambda2*dde*Rate_lambda2]
//    de(lambda2) = {[N0/((1-lambda2)*N0+lambda2*N1)]^(1/3) -[N0/N1]^(1/3)}*Q
//   dde(lambda2) = (d/dlambda2)de
//                           { h'1, for  0 <  s2^2 <  h1
//  h'(s2) = 0.5*lambda1^2 * { h'2, for h1 <= s2^2 <= h2
//                           { h'3, for h2 <  s2^2 <= 1
//     h1' = h1*a2 / [h1*s2+(t1*a2-h1)*(h1^(1/2)-s2)
//     h2' = a2/s2
//     h3' = [2*(1-s2^2)*a3]^(1/2) / s2
//   where h1,t1, h2,t2 are model parameters
//     and a2 = (h2-h1)/(t2-t1)
//         a3 = 0.5*a2^2/(1-h2)
class HotSpotCC_Rate : public HErate
{
public:
    static double DLAMBDA;              // default for dlambda
    static double TOL;                  // default for tol
private:
    HotSpotCC_Rate(const HotSpotCC_Rate &HS); // disallowed
    HotSpotCC_Rate *Duplicate();              // disallowed
    HotSpotCC_Rate *operator &();             // disallowed
    void operator=(const HotSpotCC_Rate&);    // disallowed        
protected:
    HErate *Copy(PTequilibrium *eos);   
    void PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
    void PrintParams(std::ostream &out);
    int ConvertParams(Convert &convert);
public:
    HotSpotCC_Rate();
    ~HotSpotCC_Rate();
    IDOF *Idof();
    //
    double dlambda;     // limit for TimeStep
    double tol;         // tolerance for Integrate()
    int source_type;    // return type for TimeStep()
                        //  1, dt = time_step for Rate
                        //  2, use (stiff) integrate to avoid small time step
                        //  3, Integrate() available, dt = time_step for Rate
    //
    // f(Ps)
    double Pref;        // pressure unit
    double tref;        // time unit
    double n;           // scaling exponent for deflagration speed
    // ignition regime
    double A;           // dimensionless parameter for f(Ps)
    double B;           // dimensionless parameter for f(Ps)
    // propagation regime
    double Pign,Pmax;
    double fign,fmax;
    //
    // g(s)
    double sc;          // transition point for g(s)
    double smax;        // g(s) = 1 for s>smax
    double gc,dgc,d2gc; // g0(sc), dg0/ds(sc), d2g0ds2(sc)
    double gf;          // scale factor = 1/g0(smax)
    //
    // carbon clustering energy
    double Nratio;      // N1/N0
    double eratio;      // Nratio^(-1/3)
    double Q;           // CC energy per mass
    //
    // h(t)
    double h1,t1, h2,t2;
    double tb, a2, a3, t3;
    //
    // z[] = {Ps,ts,lambda,s,s2,e_cc}
    int Rate(double V, double e, const double *z, double *zdot);
    int TimeStep(double V, double e, const double *z, double &dt);
     // dt = 0 if no restriction (lambda=1 or Ps=0 or ts<=0)
    int Integrate(double V, double e, double *z, double dt);
    int Equilibrate(double V, double e, double *z);
    //int ddz(int i, double V,double e, const double *z, double &dP,double &dT);
    double Dt(double V, double e, const double *z, double lambda);
    //
    // model specific functions
    double D(double p);             // deflagration speed, p = P/Ps
    double g(double s);             // lambda = g(s)
    double ginv(double lambda);     // ginv = s such that lambda = g(s)
    double dgds(double s);          // dg/ds
    double f(double ps);            // 
    double dfdp(double ps);         // df/dp
    double h(double t);             // lambda2 = h(t), limiting case
    double hinv(double lambda2);    // inverse function of h(t)
    double de(double lambda2);      // carbon cluster energy
    double dde(double lambda2);     // (d/dlambda) de
    double rate_s2(double lambda, double s2); // ds2/dt
    //
    // replace HErate::P(V,e,lambda), T(V,e,lambda), c(V,e,lambda)
    double P(double V, double e, double s, double s2);
    double T(double V, double e, double s, double s2);
    double c(double V, double e, double s, double s2);
};

inline double HotSpotCC_Rate::D(double p)
    { return pow(p, n); }  // p = P/Ps

#endif // EOSLIB_HOTSPOT_CC_RATE
