#ifndef EOSLIB_PHIEQ_H
#define EOSLIB_PHIEQ_H
//
#include <EOSbase.h>
//
#define PhiEq_vers "PhiEqv2.0.1"
#define PhiEq_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *PhiEqlib_vers;
extern const char *PhiEqlib_date;

inline int PhiEqlib_mismatch()
    { return strcmp(PhiEq_vers, PhiEqlib_vers); }
inline int PhiEqlib_mismatch(const char *vers)
    { return strcmp(vers, PhiEqlib_vers);       }
//
//
extern "C" {
    char *PhiEq_Init();
    char *PhiEq_Info();
}
//
//
//
class PhiEq : public EOSbase                    // abstract base class
{
public:
    static int init;
    static void Init();
 // misc
    virtual ~PhiEq();    
    PhiEq *Duplicate()      {return static_cast<PhiEq*>(EOSbase::Duplicate());} 
protected:
    double phi_min;
    double phi_max;
        
    double PV_min;
    double PV_max;
    
    double Pmin;
    double e_max;
public:
    double phi_0()     const    { return phi_min;       }
    double phi_1()     const    { return phi_max;       }

    double PV_0()      const    { return PV_min;        }
    double PV_1()      const    { return PV_max;        }

    double P_min()     const    { return phi_min*Pmin;  }
    double E_max()     const    { return e_max;         }
private:
    PhiEq();                                    // disallowed
    PhiEq(const PhiEq&);                        // disallowed
    PhiEq *operator &();                        // disallowed, use Duplicate()
    void operator=(const PhiEq&);               // disallowed
protected:
    PhiEq(const char *ptype);
    int InitBase(Parameters &, Calc &, DataBase *db=NULL);
    virtual int InitParams(Parameters &, Calc &, DataBase *db=NULL) = 0;
    virtual void PrintParams(ostream &out);
    virtual int ConvertParams(Convert &convert);    
public:
    virtual int InitPV(double PV_0, double PV_1)     = 0;
        
    virtual double phi(double PV)                    = 0;
    virtual double PV(double phi)                    = 0;
    virtual double energy(double phi)                = 0;    
    virtual int    phi(double PV,  double phi[3])    = 0;
    virtual int energy(double phi, double e[3])      = 0;
};
//
inline int deletePhiEq(PhiEq *&phi)
{
    int status = deleteEOSbase(phi,"PhiEq");
    phi = NULL;
    return status;
}
inline ostream & operator<<(ostream &out, PhiEq &phi)
            { return phi.Print(out); }
//
// Database
// 
inline PhiEq *FetchPhiEq(const char *type, const char *name, DataBase &db)
        { return (PhiEq *) db.FetchObj("PhiEq",type,name);}
inline PhiEq *FetchNewPhiEq(const char *type, const char *name, DataBase &db)
        { return (PhiEq *) db.FetchNewObj("PhiEq",type,name);}
inline PhiEq *FetchPhiEq(const char *type_name, DataBase &db)
        { return (PhiEq *) db.FetchObj("PhiEq",type_name);}
inline PhiEq *FetchNewPhiEq(const char *type_name, DataBase &db)
        { return (PhiEq *) db.FetchNewObj("PhiEq",type_name);}


#endif    // EOSLIB_PHIEQ_H
