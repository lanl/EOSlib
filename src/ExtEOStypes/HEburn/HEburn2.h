#ifndef EOSLIB_HE_BURN2
#define EOSLIB_HE_BURN2

#include <ExtEOS.h>
#include <PTequilibrium.h>
#include "HErate2.h"
//
#define HEburn2_vers "HEburn2v2.0.1"
#define HEburn2_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *HEburn2_lib_vers;
extern const char *HEburn2_lib_date;
//
extern "C" { // DataBase functions
    char *EOS_HEburn2_Init();
    char *EOS_HEburn2_Info();
    void *EOS_HEburn2_Constructor();
}
//
//  ToDo: robustness issue
//        PT equilibrium may fail when reactants go into expansion
//        Typically, for lambda > LAMBDA_MAX (0.99)
//        hook up P, T, etc. to use PTequilibrium::PTiter[1,2]
//        and if that fails replace with lambda = 1 (all burnt)
//        Temporary workaround for P & T in HEburn2Func.C
//
//  HEburn2 is extension of HEburn to case with two reactions
// Intended for carbon clustering as slow second reaction
// Second reaction shifts energy relative to equilibrium products
//

class HEburn2 : public ExtEOS
{
private:
    void operator=(const HEburn2&);      // disallowed
    HEburn2 *operator &();               // disallowed, use Duplicate()
public:
    //IDOF      *z_ref;                 // ExtEOS,  set by constructor
    //int       frozen;                 // ExtEOS
    HErate2  *rate;
    PTequilibrium *HE;
    EOS *Reactants();   // reactants = HE->eos1
    EOS *Products();    // products  = HE->eos2
    //
    // reactants -> intermediate products -> equilibrium products
    //   lambda  is first  reaction progress variable
    //   lambda2 is second reaction progress variable
    // HE is EOS for PT equilibrium of reactants and equilibrium products
    // Intermediate products have energy shift relative to equilibrium products
    // Mixture pressure is HE->P(V,e)
    //   where HE->lambda2 = lambda and HE->de = -q(lambda,lambda2)
    //   Function set_eos(lambda,lambda2) sets HE->lambda2 and HE->de
    //
    int set_eos(const double *z);
    int set_eos(double lambda, double lambda2);
    int get_lambda(const double *z, double &lambda, double &lambda2);
    int set_lambda(double lambda, double lambda2, double *z);
    int lambda_ref(double &lambda, double &lambda2) const;
    int set_lambda_ref(double lambda, double lambda2);
    void SetVerbose(int d);
protected:
    HEburn2(const HEburn2 &eos);
    void PrintParams(std::ostream &out);
    int ConvertParams(Convert &convert);
    int  PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
    //int     ExtEOS::n;                    // = z_ref::n
    //double *ExtEOS::ztmp;                 // temporary storage for IDOF
public:
    std::ostream &PrintComponents(std::ostream &out);
    HEburn2();
    ~HEburn2();
    HEburn2 *Duplicate();                    // ref_count
    ExtEOS *Copy();                         // Full copy
    EOS *CopyEOS(const double *z);          // lambda = z[ilambda]
 // ExtEOS functions
    // functions of (V,e)
    double *z_f(double V, double e);
    using ExtEOS::P;
    using ExtEOS::T;
    using ExtEOS::S;
    using ExtEOS::c2;
    using ExtEOS::Gamma;
    using ExtEOS::CV;
    using ExtEOS::CP;
    using ExtEOS::KT;
    using ExtEOS::beta;
    using ExtEOS::FD;
    using ExtEOS::e_PV;
    // z[0] = lambda
    double        P(double V, double e, const double *z);
    double        T(double V, double e, const double *z);
    double        S(double V, double e, const double *z);
    double       c2(double V, double e, const double *z);
    double    Gamma(double V, double e, const double *z);
    double       CV(double V, double e, const double *z);
    double       CP(double V, double e, const double *z);
    double       KT(double V, double e, const double *z);
    double     beta(double V, double e, const double *z);
    double       FD(double V, double e, const double *z);
    double     e_PV(double p, double V, const double *z);
    int NotInDomain(double V, double e, const double *z);
    //
    Isentrope    *isentrope   (const HydroState &state);
    Hugoniot     *shock       (const HydroState &state);
    Isotherm     *isotherm    (const HydroState &state);
    Detonation   *detonation  (const HydroState &state, double P0);
    Deflagration *deflagration(const HydroState &state, double P0);
    // functions of (const HydroState &state, const double *zi)
    using ExtEOS::isentrope;
    using ExtEOS::shock;
    using ExtEOS::isotherm;
    //
    int        Rate(double V, double e, const double *z, double *zdot);
    int    TimeStep(double V, double e, const double *z, double &dt);
    int   Integrate(double V, double e, double *z, double dt);
    int Equilibrate(double V, double e, double *z);    
    int ddz(int i, double V,double e,const double *z, double &dP,double &dT);
    int var(const char *name,double V,double e,const double *z, double &value);   
    int set_z(const char *var, double value, double *z);
    int get_z(double *z, const char *var, double &value);
    //
    double Dt(double V, double e, const double *z,
              double &lambda, double &lambda2);
};

inline HEburn2 *HEburn2::Duplicate()
    { return static_cast<HEburn2*>(EOSbase::Duplicate());  }
inline int HEburn2::set_eos(double lambda, double lambda2)
    {
        if( HE==NULL || rate==NULL ) return 1;
        HE->set_lambda2(lambda,-rate->e2(lambda2));
        return 0;
    }
inline int HEburn2::set_eos(const double *z)
    {
        if( HE==NULL || rate==NULL ) return 1;
        double lambda, lambda2;
        if( rate->get_lambda(z, lambda,lambda2) ) return 1;
        HE->set_lambda2(lambda,-rate->e2(lambda2));
        return 0;
    }
inline int HEburn2::get_lambda(const double *z, double &lambda, double &lambda2)
    { return rate ? rate->get_lambda(z,lambda,lambda2) : 1; }
inline int HEburn2::set_lambda(double lambda, double lambda2, double *z)
    { return rate ? rate->set_lambda(lambda,lambda2, z) : 1; }
inline int HEburn2::lambda_ref(double &lambda, double &lambda2) const
    { return rate ? rate->get_lambda(&(*z_ref),lambda,lambda2) : 1; }
inline int HEburn2::set_lambda_ref(double lambda, double lambda2)
    { return rate ? rate->set_lambda(lambda,lambda2, &(*z_ref)) : 1; }
inline EOS *HEburn2::Reactants()
    { return (HE && HE->eos1) ? HE->eos1->Duplicate() : NULL;}
inline EOS *HEburn2::Products()
    { return (HE && HE->eos2) ? HE->eos2->Duplicate() : NULL;}

#endif // EOSLIB_HE_BURN2
