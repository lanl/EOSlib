#ifndef EOSLIB_HE_RATE
#define EOSLIB_HE_RATE

#include <PTequilibrium.h>

#define HErate_vers "HEratev2.0.1"
#define HErate_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *HErate_lib_vers;
extern const char *HErate_lib_date;

inline int HErate_lib_mismatch()
    { return strcmp(HErate_vers, HErate_lib_vers); }
inline int HErate_lib_mismatch(const char *vers)
    { return strcmp(vers, HErate_lib_vers);       }
//
//
extern "C" {
    char *HErate_Init();
    char *HErate_Info();
}
//
//
//
class IDOF;
extern "C" void *HErate_Duplicate(void *ptr);

class HErate    : public EOSbase                // abstract base class
{
// debug
//    friend int main(int,char**);
public:
    static int init;
    static void Init();
    virtual ~HErate();    
    HErate *Duplicate();    // Should disallow ?
    PTequilibrium *HE;
    int set_lambda(double lambda);
    EOS *Reactants();   // reactants = HE->eos1, HE->lambda1 = 1-lambda
    EOS *Products();    // products  = HE->eos2, HE->lambda2 = lambda
private:
    HErate(const HErate&);                      // disallowed
    HErate *operator &();                       // disallowed
    void operator=(const HErate&);              // disallowed    
protected:
    HErate(const char *rtype);
    int InitBase(Parameters &, Calc &, DataBase *db=NULL);
    virtual void PreInit(Calc &)                                          = 0;
    virtual int PostInit(Calc &, DataBase *);
    virtual void PrintParams(std::ostream &out)                                = 0;
    virtual int ConvertParams(Convert &convert);
public:
    virtual int InitBurn(PTequilibrium *eos);
    virtual HErate *Copy(PTequilibrium *eos)                              = 0;
    double P(double V, double e, double lambda);
    double T(double V, double e, double lambda);
    double c(double V, double e, double lambda);    
    virtual IDOF *Idof()                                                  = 0;
    // z[] = {lambda, . . . }
    virtual int Rate(double V, double e, const double *z, double *zdot)   = 0;
    virtual int TimeStep(double V, double e, const double *z, double &dt) = 0;
     // dt = 0 if no restriction (lambda=1)
    virtual int Integrate(double V, double e, double *z, double dt)       = 0;
    virtual double Dt(double V, double e, const double *z, double lambda) = 0;
      // Dt = time to reach lambda
};
inline HErate *HErate::Duplicate()
    { return static_cast<HErate*>(EOSbase::Duplicate());  }  
inline EOS *HErate::Reactants()
    { return (HE && HE->eos1) ? HE->eos1->Duplicate() : NULL;}
inline EOS *HErate::Products()
    { return (HE && HE->eos2) ? HE->eos2->Duplicate() : NULL;}
//
inline int deleteHErate(HErate *&rate)
{
    int status = deleteEOSbase(rate,"HErate");
    rate = NULL;    
    return status;
}
inline ostream & operator<<(ostream &out, HErate &rate)
            { return rate.Print(out); }
//
// Database
// 
inline HErate *FetchHErate(const char *type, const char *name, DataBase &db)
            { return (HErate *) db.FetchObj("HErate",type,name);}
inline HErate *FetchNewHErate(const char *type, const char *name, DataBase &db)
            { return (HErate *) db.FetchNewObj("HErate",type,name);}
inline HErate *FetchHErate(const char *type_name, DataBase &db)
            { return (HErate *) db.FetchObj("HErate",type_name);}
inline HErate *FetchNewHErate(const char *type_name, DataBase &db)
            { return (HErate *) db.FetchNewObj("HErate",type_name);}

#endif // EOSLIB_HE_RATE
