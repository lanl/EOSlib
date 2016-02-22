#ifndef EOSLIB_DN_KAPPA_H
#define EOSLIB_DN_KAPPA_H
//
#include <iosfwd>
using std::ostream;
#include "EOSbase.h"
//
#define DnKappa_vers "DnKappav2.0.1"
#define DnKappa_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *DnKappa_lib_vers;
extern const char *DnKappa_lib_date;

inline int DnKappa_lib_mismatch()
    { return strcmp(DnKappa_vers, DnKappa_lib_vers); }
inline int DnKappa_lib_mismatch(const char *vers)
    { return strcmp(vers, DnKappa_lib_vers);       }
//
//
extern "C" { // dynamic link functions for database
    char *DnKappa_Init();
    char *DnKappa_Info();
}

class DnKappa : public EOSbase            // abstract base class
{
// ToDo: Add EOS and calculate omega_s and Dcj_r
public:
    static int init;
    static void Init();
 // misc
    virtual ~DnKappa();    
    DnKappa *Duplicate()      {return static_cast<DnKappa*>(EOSbase::Duplicate());} 
private:
    DnKappa();                                  // disallowed
    DnKappa *operator &();                      // disallowed, use Duplicate()
    void operator=(const DnKappa&);             // disallowed
protected:
    DnKappa(const char *ptype);
    DnKappa(const DnKappa&);
    int Copy(const DnKappa&, int check = 1);
    //
    int InitBase(Parameters &, Calc &, DataBase *db=NULL);
    virtual int InitParams(Parameters &, Calc &, DataBase *db=NULL) = 0;
    virtual void PrintParams(ostream &out);
    virtual int ConvertParams(Convert &convert);    
public:
    double kappa_max;       // maximum kappa
    double omega_s;         // sonic wall angle
    double Dcj;
    double d_max;           // max Dn/Dcj for kappa < 0
    //
    virtual int InitState(double rho, double T);
    virtual double Dn(double kappa) = 0;
};
//
inline int deleteDnKappa(DnKappa *&DnK)
{
    int status = deleteEOSbase(DnK,"DnKappa");
    DnK = NULL;
    return status;
}
inline ostream & operator<<(ostream &out, DnKappa &DnK)
            { return DnK.Print(out); }
//
// Database
// 
inline DnKappa *FetchDnKappa(const char *type, const char *name, DataBase &db)
        { return (DnKappa *) db.FetchObj("DnKappa",type,name);}
inline DnKappa *FetchNewDnKappa(const char *type, const char *name, DataBase &db)
        { return (DnKappa *) db.FetchNewObj("DnKappa",type,name);}
inline DnKappa *FetchDnKappa(const char *type_name, DataBase &db)
        { return (DnKappa *) db.FetchObj("DnKappa",type_name);}
inline DnKappa *FetchNewDnKappa(const char *type_name, DataBase &db)
        { return (DnKappa *) db.FetchNewObj("DnKappa",type_name);}

#endif    // EOSLIB_DN_KAPPA_H
