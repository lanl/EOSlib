#ifndef EOSLIB_IDEAL_HE_H
#define EOSLIB_IDEAL_HE_H

#include <ExtEOS.h>
#include "ArrheniusRate.h"
//
#define IdealHE_vers "IdealHEv2.0.1"
#define IdealHE_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *IdealHElib_vers;
extern const char *IdealHElib_date;
//
extern "C" { // DataBase functions
    char *EOS_IdealHE_Init();
    char *EOS_IdealHE_Info();
    void *EOS_IdealHE_Constructor();
}
//  EOS
//    F(V,T,lambda) = Cv*T*[1-ln(T/T0)-(gamma-1)*ln(V/V0)] - lambda*Q - S0*T
//    P(V,e,lambda) = (gamma-1)*(e + lambda*Q)/V
//                  = (gamma-1)*Cv*T/V
//    T(V,e,lambda) = (e + lambda*Q)/Cv
//    S(V,e,lambda) = Cv*ln[(P/P0)*(V/V0)^gamma] + S0
//                  = Cv*ln[(T/T0)*(V/V0)^(gamma-1)] + S0
//  Arrhenius kinetics (first order)
//    lambda = reaction progress variable
//           0 = reactants (unburnt)
//           1 = products (burnt)
//      Rate = (1-lambda)^n*k*exp(-T_a/T)
//           T_a = activation temperature
//           k   = rate multiplier (frequency factor)
//           n   = reaction order (0 <= n <= 1)
//
class IdealHE : public ExtEOS, public ArrheniusRate
{
private:
    void operator=(const IdealHE&);     // disallowed
    IdealHE *operator &();              // disallowed, use Duplicate()
public:
    //IDOF      *z_ref;                 // ExtEOS,  set by constructor
    //int       frozen;                 // ExtEOS
    double gamma;
    double Cv;              // specific heat
    double Q;               // specific heat release
    //
    double lambda_ref() const           { return (*z_ref)(0); }
    double lambda_ref(double lambda);   // set lambda_ref, return old value
    EOS *reactants();   // reactants: lambda = 0
    EOS *products();    //  products: lambda = 1
protected:
    IdealHE(const IdealHE&);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
    int  PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
    //int     ExtEOS::n;                // = z_ref::n
    //double *ExtEOS::ztmp;             // temporary storage for IDOF
public:
    IdealHE();
    ~IdealHE();
    IdealHE *Duplicate();           // ref_count
    ExtEOS *Copy();                 // Full copy (constructor)
    using ExtEOS::Copy;             // ExtEOS *ExtEOS::Copy(const double *z)
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
    int PT(double P, double T, HydroState &state);
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
inline double IdealHE::lambda_ref(double lambda)
{
    double lambda0=(*z_ref)(0);
    (*z_ref)[0]=lambda;
    return lambda0;
}
inline IdealHE *IdealHE::Duplicate()
    { return static_cast<IdealHE*>(EOSbase::Duplicate());  }  
//
#endif // EOSLIB_IDEAL_HE_H
