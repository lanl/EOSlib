#ifndef EOSLIB_HE_RATE2
#define EOSLIB_HE_RATE2

#include <PTequilibrium.h>

#define HErate2_vers "HErate2v2.0.1"
#define HErate2_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *HErate2_lib_vers;
extern const char *HErate2_lib_date;

inline int HErate2_lib_mismatch()
    { return strcmp(HErate2_vers, HErate2_lib_vers); }
inline int HErate2_lib_mismatch(const char *vers)
    { return strcmp(vers, HErate2_lib_vers);       }
//
//
extern "C" {
    char *HErate2_Init();
    char *HErate2_Info();
    void *HErate2_Duplicate(void *ptr);
}
//
//
class IDOF;
//
// HErate2 is extension of HErate to case with two reactions
// Intended for carbon clustering as slow second reaction
// Second reaction shifts energy relative to equilibrium products
//
class HErate2    : public EOSbase               // abstract base class
{
public:
    static int init;
    static void Init();
    virtual ~HErate2();    
    HErate2 *Duplicate();   // Note, HE variables lambda & de may change
                            // Potentially error prone, should disallow ?
    // Partly burned HE
    // reactants -> intermediate products -> equilibrium products
    //   lambda  is first  reaction progress variable
    //   lambda2 is second reaction progress variable
    // HE is EOS for PT equilibrium of reactants and equilibrium products
    // Intermediate products have energy shift relative to equilibrium products
    // Mixture pressure is HE->P(V,e)
    //   where HE->lambda2 = lambda and HE->de = -q(lambda,lambda2)/lambda
    //   Function set_eos(lambda,lambda2) sets HE->lambda2 and HE->de
    //
    PTequilibrium *HE;
    EOS *Reactants();       //             reactants = HE->eos1
    EOS *Products();        // equilibrium products  = HE->eos2
    int set_eos(const double *z);
    int set_eos(double lambda, double lambda2);
    // pressure and temperature derivative at constant V,e
    //   dP1 = dP/dlambda and dP2 = dP/dlambda2
    //   dT1 = dT/dlambda and dT2 = dT/dlambda2
    int dPdlambda(double V, double e, double lambda, double lambda2,
                                                 double &dP1, double &dP2);
    int dTdlambda(double V, double e, double lambda, double lambda2,
                                                 double &dT1, double &dT2);
    //
    virtual int set_z(const char *var, double value, double *z);
    virtual int get_z(double *z, const char *var, double &value);
    virtual int get_lambda(const double *z, double &lambda,double &lambda2)= 0;
    virtual int set_lambda(double lambda, double lambda2,  double *z)      = 0;
    // energy offset and derivatives
    // energy offset (q=0 if lambda=0 or lambda2=1)
    // dq1 = dq/dlambda and dq2 = dq/dlambda2
    virtual double q(double lambda, double lambda2)                        = 0;
    virtual double q(double lambda, double lambda2,
                                                 double &dq1, double &dq2) = 0;
    // e2 = q/lambda is used by set_eos
    virtual double e2(double lambda2)                                      = 0;
private:
    HErate2(const HErate2&);                    // disallowed
    HErate2 *operator &();                      // disallowed
    void operator=(const HErate2&);             // disallowed    
protected:
    HErate2(const char *rtype);
    int InitBase(Parameters &, Calc &, DataBase *db=NULL);
    virtual void PreInit(Calc &)                                           = 0;
    virtual int  PostInit(Calc &, DataBase *);
    virtual void PrintParams(std::ostream &out)                                 = 0;
    virtual int  ConvertParams(Convert &convert);
public:
    virtual int InitBurn(PTequilibrium *eos);
    virtual HErate2 *Copy(PTequilibrium *eos)                              = 0;
    double P(double V, double e, double lambda, double lambda2);
    double T(double V, double e, double lambda, double lambda2);
    double c(double V, double e, double lambda, double lambda2);    
    virtual IDOF *Idof()                                                   = 0;
    // z[] = {Z1, . . . , ZN}, N >= 2
    virtual int Rate(double V, double e, const double *z, double *zdot)    = 0;
    virtual int TimeStep(double V, double e, const double *z, double &dt)  = 0;
     // dt = 0 if no restriction (for example, all burnt)
    virtual int Integrate(double V, double e, double *z, double dt)        = 0;
    virtual double Dt(double V, double e, const double *z,
                                         double &lambda, double &lambda2)  = 0;
      // Dt = time to reach at least lambda and lambda2
    virtual int var(const char *name, double V, double e, const double *z,
                    double &value);
    virtual int dlambdadt(double V, double e, const double *z,
                                    double &dlambda, double &dlambda2)     = 0;
};
inline HErate2 *HErate2::Duplicate()
    { return static_cast<HErate2*>(EOSbase::Duplicate());  }  
inline EOS *HErate2::Reactants()
    { return (HE && HE->eos1) ? HE->eos1->Duplicate() : NULL;}
inline EOS *HErate2::Products()
    { return (HE && HE->eos2) ? HE->eos2->Duplicate() : NULL;}
//
inline int deleteHErate2(HErate2 *&rate2)
{
    int status = deleteEOSbase(rate2,"HErate2");
    rate2 = NULL;    
    return status;
}
inline ostream & operator<<(ostream &out, HErate2 &rate2)
            { return rate2.Print(out); }
//
// Database
// 
inline HErate2 *FetchHErate2(const char *type, const char *name, DataBase &db)
            { return (HErate2 *) db.FetchObj("HErate2",type,name);}
inline HErate2 *FetchNewHErate2(const char *type, const char *name, DataBase &db)
            { return (HErate2 *) db.FetchNewObj("HErate2",type,name);}
inline HErate2 *FetchHErate2(const char *type_name, DataBase &db)
            { return (HErate2 *) db.FetchObj("HErate2",type_name);}
inline HErate2 *FetchNewHErate2(const char *type_name, DataBase &db)
            { return (HErate2 *) db.FetchNewObj("HErate2",type_name);}

#endif // EOSLIB_HE_RATE2
