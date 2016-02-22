#ifndef EOSLIB_ELASTIC_PLASTIC_TYPES_H
#define EOSLIB_ELASTIC_PLASTIC_TYPES_H

#include "ElasticPlastic.h"
//
#define VonMises_vers "VonMisesv2.0.1"
#define VonMises_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *VonMiseslib_vers;
extern const char *VonMiseslib_date;
//
extern "C" { // DataBase functions
    char *EOS_VonMises_Init();
    char *EOS_VonMises_Info();
    void *EOS_VonMises_Constructor();
}
//
//
class VonMises : public ElasticPlastic
{
//        yield = (V/V_ref) * shear
// (d/dt)eps_pl = (3/4)*max(yield-Y,0)/nu *(J*shear/yield)
private:
    VonMises(const VonMises&);                  // for use by Duplicate only    
    void operator=(const VonMises&);            // disallowed
    VonMises *operator &();                     // disallowed, use Duplicate()
protected:
    double Y;       // yield strength
    double nu;      // shear viscosity
 // extra rate dependent parameters
    double A1;
    double A2;
    int n1;
 //
    double eps_y;
    double tau;
 //
 //  Plastic Strain Rate =  (3/4)*(Yf-Y)/nu * (J*shear/Yf)
 //                              * [1 + A1*(eps_p/eps_y)^n]
 //                              * [1 + A2*(Yf-Y)/Y]
 //                              where eps_y = Y/(2*G0)
 //                                    nu = Y*tau
 //
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
    int PreInit(Calc &);
    int PostInit(Calc &, DataBase *);

    class ZonYield;
public:
    VonMises();
    ~VonMises();
    VonMises *Duplicate()
        { return static_cast<VonMises*>(EOSbase::Duplicate());} 
    ExtEOS *Copy();

    double   yield_func(double V, double e, const double *z);
    double   yield_surf(double V, double e, const double *z);
    int         Rate(double V, double e, const double *z, double *zdot);
    int     TimeStep(double V, double e, const double *z, double &dt);
    int    Integrate(double V, double e, double *z, double dt);
    int         Step(double V, double e, double *z, double &dt);
    int  Equilibrate(double V, double e, double *z);
};

#endif // EOSLIB_ELASTIC_PLASTIC_TYPES_H
