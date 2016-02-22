#ifndef EOSLIB_VMRATE_1D_H
#define EOSLIB_VMRATE_1D_H
//
#include <Spline.h>
#include "ElasticPlastic1D.h"
#include "PlasticIDOF2.h"
#include "PolyG.h"
//
#define VMrate_vers "VMratev2.0.1"
#define VMrate_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *VMratelib_vers;
extern const char *VMratelib_date;
//
extern "C" { // DataBase functions
    char *EOS_VMrate_Init();
    char *EOS_VMrate_Info();
    void *EOS_VMrate_Constructor();
}
//
//       Yf = (3/2)*(V/V0)*abs(Pdev)    = Von Mises yield function
//       Y  = Yf(V,epsy_el)             = yield strength
// if Yf > Y => epsy_pl < eps_pl < 0 then
//   (d/dt)eps_pl   = (eps_y-eps_pl)*tau^{-1}
//   (d/dt)tau^{-1} = tau^{-1}/tau1*[(eps_pl-epsy_pl)/epsy_el]^n1
//   Orowan plastic strain rate = N*b*v with generation of dislocations
// ----------------------------------------------------------------------
//    where  epsy_pl = log(V/V0) - epsy_el is plastic strain on yield surface
//           epsy_el (assumed constant) is elastic strain on yield surface
//           tau1 is time constant
//             n1 is exponent
//
class VMrate : public ElasticPlastic1D
{
private:
    void operator=(const VMrate&);      // disallowed
    VMrate *operator &();               // disallowed, use Duplicate()
public:
    //Elastic1D *ElasticPlastic1D::elastic;
    //IDOF      *ElasticPlastic1D::z_ref;
    EOS     *hydro;             // = ElasticPlastic1D::elastic->hydro
    PolyG   *PGshear;            //(PolyG*)shear
 //
    double epsy_el;  // elastic strain on yield surface
        //   Assumes |eps_el| <= epsy_el
    double tau1;                // relaxation time constant
    double n1;                  // exponent for relaxation time
protected:
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
    int  PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
public:
    VMrate();
    VMrate(const VMrate&);    
    ~VMrate();
    VMrate *Duplicate()
        { return static_cast<VMrate*>(EOSbase::Duplicate());} 
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
 // VMrate functions // Assumes V <= V0 and eps_pl <= 0
    double Epsy_el(double V, const double *z)       // eps_el on yield surface
        { return (elastic->Eps_el(V)>z[0])?epsy_el:-epsy_el;}
    double Epsy_pl(double V, const double *z)       // eps_pl on yield surface
        {double eps_el=elastic->Eps_el(V);
         return eps_el-((eps_el>z[0])?epsy_el:-epsy_el);}
    double yield_function(double V,double eps_el)   // von Mises yield function
        {return 1.5*abs(PGshear->Pdev(V,eps_el));}
    double yield_strength(double V, const double *z)
        {return yield_function(V,Epsy_el(V,z));}
};

#endif // EOSLIB_VMRATE_1D_H
