#ifndef EOSLIB_EOS_VT_H
#define EOSLIB_EOS_VT_H

#include <EOSbase.h>
#include <String.h>

//
#define EOS_VT_vers "EOS_VTv2.0.1"
#define EOS_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *EOS_VTlib_vers;
extern const char *EOS_VTlib_date;

inline int EOS_VTlib_mismatch()
    { return strcmp(EOS_VT_vers, EOS_VTlib_vers); }
inline int EOS_VTlib_mismatch(const char *vers)
    { return strcmp(vers, EOS_VTlib_vers);}
//
class EOS;
//
class EOS_VT : public EOSbase
{
public:
    static int  init;
    static void Init();
 // reference state
    double V_ref;
    double T_ref;
 // misc
    EOS *eosVe();
    virtual ~EOS_VT();
    EOS_VT *Duplicate()    {return static_cast<EOS_VT*>(EOSbase::Duplicate());} 
private:
    EOS_VT();                                   // disallowed
    EOS_VT*operator &();                        // disallowed, use Duplicate()
    void operator=(const EOS_VT&);              // disallowed
protected:
    EOS *Ve_eos;
    virtual EOS *Ve();
    void Transfer(EOS &eos);
 //
    EOS_VT(const char *eos_type);               // eos_type must not be NULL    
    EOS_VT(const EOS_VT&);
    int Copy(const EOS_VT&, int check = 1);
    int InitBase(Parameters &, Calc &, DataBase *db=NULL);
    virtual int InitParams(Parameters &, Calc &, DataBase *db=NULL) = 0;
    virtual void  PrintParams(ostream &out);
    virtual int ConvertParams(Convert &convert);
public:
 // functions (mandatory)
    virtual double P(double V, double T)    = 0;    // Pressure
    virtual double e(double V, double T)    = 0;    // specific energy
    virtual double S(double V, double T)    = 0;    // entropy
    virtual double P_V(double V, double T)  = 0;    // dP/dV @ const T
    virtual double P_T(double V, double T)  = 0;    // dP/dT @ const V
    virtual double CV(double V, double T)   = 0;    // T*(d/dT)S @ const V
    // Thermodynamic relations
    //  cT^2 = -V^2 dP/d(V)|_S      = isothermal sound speed squared
    //  c^2  = cT^2 + Gamma^2 C_V T = isentropic sound speed squared 
    // V dP =                  -cT^2 dV/V + Gamma C_V dT
    //   de = -[1-Gamma C_V T /(PV)] P dV +       C_V dT
    // T dS =     Gamma C_V T        dV/V +       C_V dT
 // functions (default supplied)
    virtual double F(double V, double T);       // F = e - T*S
                                                // dF = -P dV - T dS
    virtual int NotInDomain(double V, double T);
    int InDomain(double V, double T) { return !NotInDomain(V,T); }
    virtual double CP(double V, double T);      // Specific heat at constant P
    virtual double KS(double V, double T);      // Isentropic bulk modulus
    virtual double KT(double V, double T);      // Isothermal bulk modulus
    virtual double Gamma(double V, double T);   // -(V/T)dT/dV @ const S
    virtual double beta(double V, double T);    // volumetric thermal expansion
       
    virtual double c2(double V, double T);      // isentropic (sound speed)^2
    virtual double cT2(double V, double T);     // isothermal (sound speed)^2
    double c(double V, double T);               // isentropic sound speed
    double cT(double V, double T);              // isothermal sound speed    
};
//
inline int deleteEOS_VT(EOS_VT *&eos)
{
    int status = deleteEOSbase(eos,"EOS_VT");
    eos = NULL;
    return status;
}
//
// Database
// 
inline EOS_VT *FetchEOS_VT(const char *type, const char *name, DataBase &db)
            { return (EOS_VT *) db.FetchObj("EOS_VT",type,name);}
inline EOS_VT *FetchNewEOS_VT(const char *type, const char *name, DataBase &db)
            { return (EOS_VT *) db.FetchNewObj("EOS_VT",type,name);}
inline EOS_VT *FetchEOS_VT(const char *type_name, DataBase &db)
            { return (EOS_VT *) db.FetchObj("EOS_VT",type_name);}
inline EOS_VT *FetchNewEOS_VT(const char *type_name, DataBase &db)
            { return (EOS_VT *) db.FetchNewObj("EOS_VT",type_name);}
// print
inline ostream & operator<<(ostream &out, EOS_VT &eos)
            { return eos.Print(out); }
//
// inline EOS_VT functions
//
inline double EOS_VT::c(double V, double T)
{
    double csq = c2(V,T);
    return (isnan(csq) || csq < 0) ? NaN : sqrt(csq);
}
inline double EOS_VT::cT(double V, double T)
{
    double csq = cT2(V,T);
    return (isnan(csq) || csq < 0) ? NaN : sqrt(csq);
}

#endif // EOSLIB_EOS_VT_H
