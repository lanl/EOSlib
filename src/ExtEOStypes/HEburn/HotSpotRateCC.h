#ifndef EOSLIB_HOTSPOT_RATE_CC
#define EOSLIB_HOTSPOT_RATE_CC

#include <HErate2.h>
//
#define HotSpotRateCC_vers "HotSpotRateCC_v2.0.3"
#define HotSpotRateCC_date "Sept 7, 2015"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *HotSpotRateCC_lib_vers;
extern const char *HotSpotRateCC_lib_date;
//
extern "C" { // DataBase functions
    char *HErate2_HotSpotRateCC_Init();
    char *HErate2_HotSpotRateCC_Info();
    void *HErate2_HotSpotRateCC_Constructor();
}
//
//  Hot spot + carbon cluster reaction rates (SURF-plus model)
//  reactants -> intermediate products -> equilibrium products
//  ----------------------------------------
//  lambda  = hot spot reaction progress variable
//           0 = reactants (unburnt)
//           1 = products  (no reactants)
//  lambda2 = carbon cluster reaction progress variable
//           0 = no carbon clusters
//           1 = equilibrium carbon clusters
//  ----------------------------------------
//  HotSpotCC_HE IDOF
//      z[0] = Ps = lead shock pressure     parameter supplied by hydro code
//      z[1] = t  = time - ts               parameter supplied by hydro code
//             time is that of hydro code
//               ts is shock arrival time
//      z[2] = s,  scaled first  reaction variable  (dimensionless)
//      z[3] = s2, scaled second reaction variable  (dimensionless)
//
//  Rate_s  = (d/dt) s
//          = f(Ps)*max(0,min(1,[p(t)/Ps]^n)),  for t > 0
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
//          = P(V, e-lambda*e_cc(lambda2), lambda)
//  P(V,e,lambda) is P-T equilibium of reactants and equilibrium products
//     e_cc(lambda2) is model function for carbon cluster energy
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
//  Old form
//  parameters: A,B, Pign,Pmax, fmax
//              = exp(A+B*Ps/Pref),                          for       Ps < Pign
//  t_ref*f(Ps) = [(P-Pign)*fmax+(Pmax-P)*fign]/(Pmax-Pign), for Pign < Ps <Pmax
//              = fmax,                                      for Pmax < Ps   
//   (Pref, tref) correspond to choice of units
//
//  New form
//  parameters: A,B, P0,P1, df1
//        =  0,                                          for Ps < P0
//  f(Ps) = f0(Ps) - f0(P0)*(1+B*(Ps-P0)/Pref),          for P0 < Ps < P1
//        = f1*(1+df1*[1-exp(-B2*(Ps-P1)/Pref)],         for      P1 < Ps
//  where
//  f0(Ps) = exp(A+B*Ps/Pref) / tref
//     f1  = f(P1) = f0(P1) - f0(P0)*(1+B*(P1-P0)/Pref)
//     B2  = Pref*(df/dPs)(P1)/(f1*df1)
//  f and df/dPs continuous at P0 and P1
//
//  Alt form
//  parameters Cr,Pr, P0,P1, fn, df1
//              =  0,                                    for Ps < P0
//  t_ref*f(Ps) = C*[(Ps-P0)/Pref]**fn,                  for P0 < Ps < P1
//              = f1*(1+df1*[1-exp(-B2*(Ps-P1)/Pref)],   for      P1 < Ps
// where
//    C  = Cr/[(Pr-P0)/Pref]**fn
//    B2 = (fn/df1)*(Pref/(P1-P0))
//  f and df/dPs continuous at P0 and P1, if fn>1

//
// functions for carbon cluster model
//    de(lambda2) = {[N0/((1-lambda2)*N0+lambda2*N1)]^(1/3) -[N0/N1]^(1/3)}*Q
//                  is carbon cluster energy relative to equilibrium
//   dde(lambda2) = (d/dlambda2)de
//   q = lambda*de(lambda2) is energy shift relative to equilibrium products
//        = 0 unburned (lambda=0) and fully burned (lambda=lambda2=1)
//
// Mixture EOS
//     P(V,e,lambda,lambda2) = HE->P(V,e)
//     HE->P is PT equilibrium of reactants and equilibrium products
//      with HE->lambda2 = lambda and HE->de = -q
//     or energy release in hydro code reaction step
//        (d/dt)e + P*(d/dt)V = -(d/dt) q
//                = -[de(lambda2)*Rate_lambda + lambda2*dde*Rate_lambda2]
//
//                           { h'1, for  0 <  s2^2 <  h1
//  h'(s2) = 0.5*lambda1^2 * { h'2, for h1 <= s2^2 <= h2
//                           { h'3, for h2 <  s2^2 <= 1
//     h1' = h1*a2 / [h1*s2+(t1*a2-h1)*(h1^(1/2)-s2)
//     h2' = a2/s2
//     h3' = [2*(1-s2^2)*a3]^(1/2) / s2
//   where h1,t1, h2,t2 are model parameters
//     and a2 = (h2-h1)/(t2-t1)
//         a3 = 0.5*a2^2/(1-h2)
//
class HotSpotRateCC : public HErate2
{
public:
    static double DLAMBDA;              // default for dlambda
    static double TOL;                  // default for tol
private:
    HotSpotRateCC(const HotSpotRateCC &HS); // disallowed
    HotSpotRateCC *Duplicate();              // disallowed
    HotSpotRateCC *operator &();             // disallowed
    void operator=(const HotSpotRateCC&);    // disallowed        
protected:
    HErate2 *Copy(PTequilibrium *eos);   
    void PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
    void PrintParams(std::ostream &out);
    int ConvertParams(Convert &convert);
public:
    HotSpotRateCC();
    ~HotSpotRateCC();
    IDOF *Idof();
    //
    double dlambda;     // limit for TimeStep
    double tol;         // tolerance for Integrate()
    int source_type;    // return type for TimeStep()
                        //  1, dt = time_step for Rate
                        //  2, use (stiff) integrate to avoid small time step
                        //  3, Integrate() available, dt = time_step for Rate
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
    // new f(Ps) with smooth cutoff
    double P0, P1;
    double f0, f1;
    double df1, B2;
    // Alt new form
    double Cr,Pr, C, fn;
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
    // z[] = {Ps,ts,s,s2}
    int set_z(const char *var, double value, double *z);
    int get_z(double *z, const char *var, double &value);
    int get_lambda(const double *z, double &lambda,double &lambda2);
    int set_lambda(double lambda, double lambda2,  double *z);
    // energy offset and derivatives
    // dq1 = dq/dlambda and dq2 = dq/dlambda2
    double q(double lambda, double lambda2);
    double q(double lambda, double lambda2, double &dq1, double &dq2);
    // e2 = q/lambda is used by set_eos
    double e2(double lambda2);
    //
    using HErate2::P;
    using HErate2::T;
    using HErate2::c;
    //
    int Rate(double V, double e, const double *z, double *zdot);
    int TimeStep(double V, double e, const double *z, double &dt);
     // dt = 0 if no restriction (lambda=1 or Ps=0 or ts<=0)
    int Integrate(double V, double e, double *z, double dt);
    int Equilibrate(double V, double e, double *z);
    double Dt(double V, double e, const double *z,
                                  double &lambda, double &lambda2);
    //
    // model specific functions
    double D(double p);             // deflagration speed, p = P/Ps
    double g(double s);             // lambda = g(s)
    double ginv(double lambda);     // ginv = s such that lambda = g(s)
    double dgds(double s);          // dg/ds
    double f(double Ps);            // 
    double dfdp(double Ps);         // df/dp
    double h(double t);             // lambda2 = h(t), limiting case
    double hinv(double lambda2);    // hinv = t such that lambda2 = h(t)
    double rate_s(double V, double e, const double *z);   // ds/dt
    double rate_s2(double lambda, double s2);       // ds2/dt

    int var(const char *name, double V,double e,const double *z, double &value);
    int dlambdadt(double V, double e, const double *z,
                                      double &dlambda, double &dlambda2);
};

inline double HotSpotRateCC::D(double p)
    { return (p>1.) ? p : (p>0.) ? pow(p, n) : 0.; }  // p = P/Ps

#endif // EOSLIB_HOTSPOT_RATE_CC
