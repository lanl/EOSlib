#ifndef EOSLIB_POLYMER_1D_H
#define EOSLIB_POLYMER_1D_H
//
#include <Spline.h>
#include "ElasticPlastic1D.h"
#include "PlasticIDOF.h"
#include "PolyG.h"
//
#define Polymer_vers "Polymerv2.0.1"
#define Polymer_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *Polymerlib_vers;
extern const char *Polymerlib_date;
//
extern "C" { // DataBase functions
    char *EOS_Polymer_Init();
    char *EOS_Polymer_Info();
    void *EOS_Polymer_Constructor();
}
//
//       Yf     = (3/2)*(V/V0)*abs(Pdev)       = Von Mises yield function 
// (d/dt)eps_pl = sign(eps_el)*(eps_pl-eps_y)/tau = plastic-strain rate
// ----------------------------------------------------------------------
//    where  tau = tau_0 / {1 + [(eps_pl-eps_y)/eps_0]^n}
//       eps_y  = plastic strain on yield surface
//        tau_0 is relaxation time constant
//        eps_y(eta) where eta = 1-V/V0
//
class Polymer : public ElasticPlastic1D
{
private:
    void operator=(const Polymer&);      // disallowed
    Polymer *operator &();               // disallowed, use Duplicate()
protected:
    Espline eps_y;  // |eps_pl|(1-V/V0) on yield surface for V <= V0
    // ToDo:
    // replace eps_y with epsy_min and epsy_max for eps_el on yield surface
    // where yield_strength=yield_function(epsy_min)=yield_function(epsy_max)
    // extend yield_strength to expansion as well as compression
    int Reinit();
public:
    //Elastic1D *ElasticPlastic1D::elastic;
    //IDOF      *ElasticPlastic1D::plastic_ref;
    EOS     *hydro;             // = ElasticPlastic1D::elastic->hydro
    PolyG   *Pshear;            //(PolyG*)shear
 //
    double  *epsy_data;         // data for eps_y spline
    int     n_data;             // number of data points (knots)
    double  depsdeta0;          // d(dps_y)/d(eta) at eta=0
    double  depsdeta1;          // d(dps_y)/d(eta) at eta=1-Vmin/V0
    double  Vmin;               // spline interval Vmin <= V <= V0
 //
    double tau0;                // relaxation time constant
    double eps0;                // strain scale
    double n;                   // exponent for relaxation time
protected:
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
    int  PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
public:
    Polymer();
    Polymer(const Polymer&);    
    ~Polymer();
    Polymer *Duplicate()
        { return static_cast<Polymer*>(EOSbase::Duplicate());} 
    ExtEOS *Copy();
 // ExtEOS functions
    int        Rate(double V, double e, const double *z, double *zdot);
    int    TimeStep(double V, double e, const double *z, double &dt);
    int   Integrate(double V, double e, double *z, double dt);
    int Equilibrate(double V, double e, double *z);
 // ElasticPlastic1D
    double yield_function(double V, double e, const double *z);
    double yield_strength(double V, double e, const double *z);
public:
 // Polymer functions // Assumes V <= V0 and eps_pl <= 0
    double epsy_pl(double V)                        // eps_pl on yield surface
        {  if( V > 1.001*V_ref) return NaN;
           if( V > V_ref ) V = V_ref;
           return -eps_y.Evaluate(1.-V/V_ref);}
    double epsy_el(double V)                        // eps_el on yield surface
        {return elastic->Eps_el(V)-epsy_pl(V);}
    double yield_function(double V,double eps_el)   // von Mises yield function
        {return 1.5*abs(Pshear->Pdev(V,eps_el));}
    double yield_strength(double V)
        {return yield_function(V,epsy_el(V));}
    double Tau(double eps_pl, double V);            // time constant
};

#endif // EOSLIB_POLYMER_1D_H
