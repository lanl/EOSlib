#ifndef EOSLIB_VON_MISES_1D_H
#define EOSLIB_VON_MISES_1D_H
//
#include "ElasticPlastic1D.h"
#include "PlasticIDOF.h"
#include <OneDFunction.h>
//
#define VonMises1D_vers "VonMises1Dv2.0.1"
#define VonMises1D_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *VonMises1Dlib_vers;
extern const char *VonMises1Dlib_date;
//
extern "C" { // DataBase functions
    char *EOS_VonMises1D_Init();
    char *EOS_VonMises1D_Info();
    void *EOS_VonMises1D_Constructor();
}
//
//       Yf     = (3/2)*(V/V0)*abs(Pdev)       = Von Mises yield function 
// (d/dt)eps_pl = sign(eps_el)*(1/2)*(Yf-Y)/Nu = plastic-strain rate
// ----------------------------------------------------------------------
//    where  Nu = nu / [1 + A1*(eps_p/eps_y)^n1 + A2*(Yf/Y-1)^n2]
//     Y(V,e,z) = yield strength = Y0
//       eps_y  = 0.5*Y0/G0, scale for plastic strain
//        nu is parameter with dimensions of viscosity (pressure*time)
//  Alt. Y(V,e,z) = Yf(V,e,eps_y), ie, constant elastic strain on yield surface
//  Von Mises Yield condition
//  -------------------------
//      Y^2 = (3/2) (V/V0)^2||dev sigma||^2
//      Y   = (3/2)*(V/V0)*abs(dev(sigma)^{xx})
//          = (3/2)*(V/V0)*abs(elastic->Pdev) = Yf
//  If Yf ~> Y and (d/dt) eps_total < 1/(plastic time constant)
//  Then approximately rate independent plasticity (Y = Yf)
//       + viscous shear stress = Nu*dev[grad(u)+grad(u)^T]
//  Plastic time constant = Nu/G, relaxation to yield surface
//  1-D, sigma^xx = (4/3)*Nu*(du/dx), u is x-velocity
//  Zero yield strength (Y=0) corresponds to visco-elastic model
//
class VonMises1D : public ElasticPlastic1D, protected OneDFunction
{
private:
    void operator=(const VonMises1D&);      // disallowed
    VonMises1D *operator &();               // disallowed, use Duplicate()
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
protected:
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
    int  PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
public:
    VonMises1D();
    VonMises1D(const VonMises1D&);    
    ~VonMises1D();
    VonMises1D *Duplicate()
        { return static_cast<VonMises1D*>(EOSbase::Duplicate());} 
    ExtEOS *Copy();
 // ExtEOS functions
    int        Rate(double V, double e, const double *z, double *zdot);
    int    TimeStep(double V, double e, const double *z, double &dt);
    int   Integrate(double V, double e, double *z, double dt);
    int Equilibrate(double V, double e, double *z);
 // ElasticPlastic1D
    double yield_function(double V, double e, const double *z);
    double yield_strength(double V, double e, const double *z);// Y=Y(V,e,z)
protected:
 // VonMises1D functions
    double yield_function(double V, double Tve, const double eps_el)
        { return 1.5*(V/V_ref)*abs(shear->Pdev(V,Tve,eps_el));  }
    double Nu(double eps_pl, double Y, double Yf);
    double V1, e1, eps_V1, Y1;      // internal storage for Step & f
    int Step(double &eps_el, double &dt);
    double f(double eps_el);        // OneDFunction::f = yield_function
};

#endif // EOSLIB_VON_MISES_1D_H
