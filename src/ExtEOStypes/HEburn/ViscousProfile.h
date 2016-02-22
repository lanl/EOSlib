// Viscous profile for Forest Fire rate
#include <LocalMath.h>
#include <ODE.h>
//
#include "HEburn.h"
#include "FFrate.h"
//
class ViscousProfile : public ODE
{
// xi = x - D*t <= 0, right facing wave
// m = rh0_0*D = mass flux
// nu (d/dxi) u = Delta(P) - m Delta(u)
// (d/dxi) lambda = -Rate/(m V)
//            V  = V0 - (D-u)/m
//            e  = e0 + (P_0/m + 0.5*Delta(u))* Delta(u)
//            P  = P(V,e,lambda)
//  (d/dxi) t = 1/(D-u)     // Lagrangian time
//
public:
    FFrate &FF;
    EOS *reactants;
    EOS *products;
    double nu;              // viscous coefficient (dimensionless)
    double r;               // ratio of quadratic to linear term
    double len;             // length scale for viscosity
    double Q(double V,double e,double lambda, double dudx);  // viscous pressure
    double Pvn;         // pressure at von Neumann spike
    double Pmin;        // pressure at start of profile (skip tail)
    double Umin;        // u corresponding to Pmin
    double D;           // wave speed
    double m;           // mass flux
    WaveState ws_0;     // initial state
    WaveState ws_cj;    // CJ state
    WaveState ws_vn;    // wave state at von Neumann spike
    WaveState ws_w;     // wave state on   weak branch of detonation locus
    WaveState ws_s;     // wave state on strong branch of detonation locus
    ViscousProfile(FFrate *rate, double pvn=0., double umin=0.0 );
    ~ViscousProfile();
    int Reinit();
    int Reinit(double nup,double lenp=0.,double rp=0.);
    int F(double *y_prime, const double *y_xi, double xi); // ODE::F 
    double y[3], yp[3];
    double V,e,p,q,rate;
    void Hydrostate(double u, double lambda);  // set V,e,P,q
    void Hydrostate() { Hydrostate(y[0],y[1]);}
    int Lambda(double &lambda, double &xi);
    int P(double &pxi, double &xi);
    //
    class lambda_ODE;
    class      P_ODE;
    //
    int status;             // error status
    double xi0, p0, t0;     // origin for xi
    std::ostream &Header(std::ostream &out, double p0=0.1);
    std::ostream &Print(std::ostream &out, double xi);
    std::ostream &Print(std::ostream &out, double xi,double u, double lambda, double t);
};
