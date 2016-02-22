#ifndef EOSLIB_VON_MISES_CONSTGV_1D_H
#define EOSLIB_VON_MISES_CONSTGV_1D_H
//
#include "ElasticPlastic1D.h"
#include "PlasticIDOF.h"
#include <OneDFunction.h>
//
#define VonMisesConstGV_vers "VonMisesConstGVv2.0.1"
#define VonMisesConstGV_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *VonMisesConstGVlib_vers;
extern const char *VonMisesConstGVlib_date;
//
extern "C" { // DataBase functions
    char *EOS_VonMisesConstGV_Init();
    char *EOS_VonMisesConstGV_Info();
    void *EOS_VonMisesConstGV_Constructor();
}
//
//       Yf     = (3/2)*(V/V0)*abs(Pdev)        = Von Mises yield function 
// (d/dt)eps_pl = sign(eps_el)*(1/2)*(Yf-Y)/Nu  = plastic-strain rate
// -----------------------------------------------------------------------
//   where   Nu = nu / [1 + A1*(eps_p/eps_y)^n1 + A2*(Yf/Y-1)^n2]
//     Y(V,e,z) = Y0 = yield strength
//       eps_y  = 0.5*Y0/G0, scale for plastic strain
//        nu is parameter with dimensions of viscosity (pressure*time)
//  Specialized to elastic->shear of type Shear1::ConstGV
//                 --------------         ---------------    
//       Yf is independent of (V,e) and
//       constant elastic strain on yield surface
//       ----------------------------------------
//           eps_y_compression in compression
//           eps_y_expansion in expansion       
//  Von Mises Yield condition
//  -------------------------
//      Y^2 = (3/2) (V/V0)^2||dev sigma||^2
//      Y   = (3/2)*(V/V0)*abs(dev(sigma)^{xx})
//          = (3/2)*(V/V0)*abs(elastic->Pdev) = Yf(eps_el)
//  Zero yield strength (Y=0) corresponds to visco-elastic model
//
class VonMisesConstGV : public ElasticPlastic1D, protected OneDFunction
{
private:
    void operator=(const VonMisesConstGV&);      // disallowed
    VonMisesConstGV *operator &();               // disallowed, use Duplicate()
public:
    //Elastic1D *ElasticPlastic1D::elastic;
    //IDOF      *ElasticPlastic1D::plastic_ref;
    EOS     *hydro;             // = ElasticPlastic1D::elastic->hydro
    Shear1D *shear;             // = ElasticPlastic1D::elastic->shear  
    double Y0;                  // yield strength
    double nu;                  // shear viscosity
 // extra rate dependent parameters
    double A1;
    double A2;
    double n1;
    double n2;
 // derived quantities 
    double eps_y;               // 0.5*Y0/G0, scale for plastic strain
    double G0;                  // ConstGV->GV/V_ref
    double gv;                  // ConstGV->GV
    double eps_y_compression;   // eps_el on yield surface in compression
    double eps_y_expansion;     // eps_el on yield surface in expansion
protected:
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
    int PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
public:
    VonMisesConstGV();
    VonMisesConstGV(const VonMisesConstGV&);    
    ~VonMisesConstGV();
    VonMisesConstGV *Duplicate()
        { return static_cast<VonMisesConstGV*>(EOSbase::Duplicate());} 
    ExtEOS *Copy();
 // ExtEOS functions
    int        Rate(double V, double e, const double *z, double *zdot);
    int    TimeStep(double V, double e, const double *z, double &dt);
    int   Integrate(double V, double e, double *z, double dt);
    int Equilibrate(double V, double e, double *z);
 // ElasticPlastic1D
    double yield_function(double V, double e, const double *z);
    double yield_strength(double V, double e, const double *z); // Y=Y(V,e,z)
protected:
 // VonMisesConstGV functions
    double Yfunction(double eps_el)
        { double x=exp((2./3.)*eps_el); return G0*abs(x*x-1/x);         }   
    double GV(double eps_el)    // (3/4)*V*shear->dPdev_deps
        { double x=exp((2./3.)*eps_el); return (1./3.)*gv*(2*x*x+1/x);  }   
    double f(double eps_el);    // OneDFunction::f = Yfunction
    double V1, eps_V1;
    int Step(double &eps_el, double &dt);
};

#endif // EOSLIB_VON_MISES_CONSTGV_1D_H
