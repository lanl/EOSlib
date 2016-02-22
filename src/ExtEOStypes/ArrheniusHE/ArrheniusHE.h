#ifndef EOSLIB_ARRHENIUS_HE_H
#define EOSLIB_ARRHENIUS_HE_H

#include <ExtEOS.h>
#include <PTequilibrium.h>
#include "ArrheniusRate.h"
//
#define ArrheniusHE_vers "ArrheniusHEv2.0.1"
#define ArrheniusHE_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *ArrheniusHElib_vers;
extern const char *ArrheniusHElib_date;
//
extern "C" { // DataBase functions
    char *EOS_ArrheniusHE_Init();
    char *EOS_ArrheniusHE_Info();
    void *EOS_ArrheniusHE_Constructor();
}
//
//  ArrheniusHE HE kinetics
//  lambda = reaction progress variable
//          0 = reactants (unburnt)
//          1 = products (burnt)
//  Rate = (1-lambda)^n*k*exp(-T_a/T)
//         T_a = activation temperature
//         k   = rate multiplier (frequency factor)
//         n   = reaction order (0 <= n <= 1)
//
class ArrheniusHE : public ExtEOS, public ArrheniusRate
{
private:
    void operator=(const ArrheniusHE&);     // disallowed
    ArrheniusHE *operator &();              // disallowed, use Duplicate()
public:
    //IDOF      *z_ref;                     // ExtEOS,  set by constructor
    //int       frozen;                     // ExtEOS
    PTequilibrium *HE;
    EOS *reactants();   // reactants = HE->eos1, HE->lambda1 = 1-lambda
    EOS *products();    // products  = HE->eos2, HE->lambda2 = lambda
    int set_lambda(double lambda);  // HE->lambda2;
    double get_lambda();            // HE->lambda2;
    double lambda_ref() const           { return (*z_ref)(0); }
    double lambda_ref(double lambda);   // set lambda_ref, return old value
protected:
    ArrheniusHE(const ArrheniusHE&);
    void PrintParams(std::ostream &out);
    int ConvertParams(Convert &convert);
    int  PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
    //int     ExtEOS::n;                    // = z_ref::n
    //double *ExtEOS::ztmp;                 // temporary storage for IDOF
public:
    std::ostream &PrintComponents(std::ostream &out);
    ArrheniusHE();
    ~ArrheniusHE();
    ArrheniusHE *Duplicate();               // ref_count
    ExtEOS *Copy();                         // Full copy (constructor)
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
    using ArrheniusRate::Rate;
    int ddlambda(double V, double e, double lambda, double &dP, double &dT);
    double Dt(double V, double e, const double *z, double lambda);
};
//
inline ArrheniusHE *ArrheniusHE::Duplicate()
    { return static_cast<ArrheniusHE*>(EOSbase::Duplicate());  }  
inline double ArrheniusHE::lambda_ref(double lambda)
{
    double lambda0=(*z_ref)(0);
    (*z_ref)[0]=lambda;
    return lambda0;
}
inline EOS *ArrheniusHE::reactants()
    { return (HE && HE->eos1) ? HE->eos1->Duplicate() : NULL;}
inline EOS *ArrheniusHE::products()
    { return (HE && HE->eos2) ? HE->eos2->Duplicate() : NULL;}

#endif // EOSLIB_ARRHENIUS_HE_H
