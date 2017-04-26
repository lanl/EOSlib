#ifndef EOSLIB_HE_BURN_H
#define EOSLIB_HE_BURN_H

#include <ExtEOS.h>
#include <PTequilibrium.h>
#include "HErate.h"
//
#define HEburn_vers "HEburnv2.0.1"
#define HEburn_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *HEburn_lib_vers;
extern const char *HEburn_lib_date;
//
extern "C" { // DataBase functions
    char *EOS_HEburn_Init();
    char *EOS_HEburn_Info();
    void *EOS_HEburn_Constructor();
}
//
//  ToDo: robustness issue
//        PT equilibrium may fail when reactants go into expansion
//        Typically, for lambda > LAMBDA_MAX (0.99)
//        hook up P, T, etc. to use PTequilibrium::PTiter[1,2]
//        and if that fails replace with lambda = 1 (all burnt)
//        Temporary workaround for P & T in HEburnFunc.C
//
class HEburn : public ExtEOS
{
private:
    void operator=(const HEburn&);      // disallowed
    HEburn *operator &();               // disallowed, use Duplicate()
public:
    //IDOF      *z_ref;                 // ExtEOS,  set by constructor
    //int       frozen;                 // ExtEOS
    HErate  *rate;
    PTequilibrium *HE;
    EOS *Reactants();   // reactants = HE->eos1, HE->lambda1 = 1-lambda
    EOS *Products();    // products  = HE->eos2, HE->lambda2 = lambda
    int ilambda;                        // index of lambda
    int set_lambda(double lambda);      // HE->lambda2;
    double get_lambda();                // HE->lambda2;
    double lambda_ref() const           { return (*z_ref)(ilambda); }
    double lambda_ref(double lambda);   // set lambda_ref, return old value
    void SetVerbose(int d);
protected:
    HEburn(const HEburn &eos);
    void PrintParams(std::ostream &out);
    int ConvertParams(Convert &convert);
    int  PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
    //int     ExtEOS::n;                    // = z_ref::n
    //double *ExtEOS::ztmp;                 // temporary storage for IDOF
public:
    std::ostream &PrintComponents(std::ostream &out);
    HEburn();
    ~HEburn();
    HEburn *Duplicate();                    // ref_count
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
    //
    int ddlambda(double V, double e, double lambda, double &dP, double &dT);
    double Dt(double V, double e, const double *z, double lambda);
};

inline HEburn *HEburn::Duplicate()
    { return static_cast<HEburn*>(EOSbase::Duplicate());  }  
inline double HEburn::lambda_ref(double lambda)
{
    double lambda0=(*z_ref)(ilambda);
    (*z_ref)[ilambda]=lambda;
    return lambda0;
}
inline EOS *HEburn::Reactants()
    { return (HE && HE->eos1) ? HE->eos1->Duplicate() : NULL;}
inline EOS *HEburn::Products()
    { return (HE && HE->eos2) ? HE->eos2->Duplicate() : NULL;}

#endif // EOSLIB_HE_BURN_H
