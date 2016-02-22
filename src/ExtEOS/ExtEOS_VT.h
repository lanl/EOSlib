#ifndef EOSLIB_EXT_EOS_VT_H
#define EOSLIB_EXT_EOS_VT_H

#include <EOS_VT.h>
#include "IDOF.h"

// base class for Extended EOS_VT
//      EOS_VT with internal degrees of freedom (IDOF)
//      and rate equation for evolution of IDOF
class ExtEOS_VT : public EOS_VT
{
private:
    void operator=(const ExtEOS_VT&);       // disallowed
    ExtEOS_VT *operator &();                // disallowed, use Duplicate()
protected:
    ExtEOS_VT(const char *eos_type);    
    ExtEOS_VT(const ExtEOS_VT&);    
    // int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    // void PrintParams(ostream &out);
    // int ConvertParams(Convert &convert);        
    int n;          // number of internal degrees of freedom
    double *ztmp;   // temporary storage for internal degrees of freedom
public:
    int N() const       { return n;         }
    double &Z(int i)    { return ztmp[i];   }
    double *Z()         { return ztmp;      }
    virtual IDOF *z_ref();
    virtual double *z_f(double V, double T); // frozen or equilibrium z
    virtual ~ExtEOS_VT();
    ExtEOS_VT *Duplicate();
    virtual ExtEOS_VT *Duplicate(const double *z);
    // ostream &PrintComponents(ostream &out);
 // EOS_VT functions
    virtual double     P(double V, double T);
    virtual double     e(double V, double T);
    virtual double     S(double V, double T);
    virtual double   P_V(double V, double T);
    virtual double   P_T(double V, double T);
    virtual double    CV(double V, double T);
    virtual double     F(double V, double T);               // F = e - T*S
    virtual int NotInDomain(double V, double e);
    
    virtual double    CP(double V, double T);
    virtual double    KS(double V, double T);
    virtual double    KT(double V, double T);
    virtual double Gamma(double V, double T);
    virtual double  beta(double V, double T);
    virtual double    c2(double V, double T);
    // using EOS_VT::cT2;
    // using EOS_VT::c;
    // using EOS_VT::cT;
    virtual double cT2(double V, double T);     // isothermal (sound speed)^2
    double c(double V, double T);               // adiabatic sound speed
    double cT(double V, double T);              // isothermal sound speed        
 // Extended functionality
    virtual double     P(double V, double T, const double *z) = 0;
    virtual double     e(double V, double T, const double *z);
    virtual double     S(double V, double T, const double *z);
    virtual double   P_V(double V, double T, const double *z);
    virtual double   P_T(double V, double T, const double *z);
    virtual double    CV(double V, double T, const double *z);
    virtual double     F(double V, double T, const double *z);    // F = e - T*S
    virtual int NotInDomain(double V, double e, const double *z);
    
    virtual double    CP(double V, double T, const double *z);
    virtual double    KS(double V, double T, const double *z);
    virtual double    KT(double V, double T, const double *z);
    virtual double Gamma(double V, double T, const double *z);
    virtual double  beta(double V, double T, const double *z);
    virtual double    c2(double V, double T, const double *z);
    virtual double   cT2(double V, double T, const double *z);
            double     c(double V, double T, const double *z);
            double    cT(double V, double T, const double *z);
 // 
    virtual int Rate(double V, double T, const double *z, double *zdot) = 0;
};

inline ExtEOS_VT *ExtEOS_VT::Duplicate()
{
    return static_cast<ExtEOS_VT*>(EOSbase::Duplicate());
}
//
inline int deleteExtEOS_VT(ExtEOS_VT *&eos)
{
    int status = deleteEOSbase(eos,"ExtEOS_VT");
    eos = NULL;
    return status;
}
//
// Database
// 
ExtEOS_VT *FetchExtEOS_VT(const char *type, const char *name, DataBase &db);
ExtEOS_VT *FetchNewExtEOS_VT(const char *type, const char *name, DataBase &db);
ExtEOS_VT *FetchExtEOS_VT(const char *type_name, DataBase &db);
ExtEOS_VT *FetchNewExtEOS_VT(const char *type_name, DataBase &db);
// print
inline std::ostream & operator<<(std::ostream &out, ExtEOS_VT &eos)
            { return eos.Print(out); }
//
inline double ExtEOS_VT::c(double V, double T, const double *z)
{
    double csq = c2(V, T, z);
    return (isnan(csq) || csq < 0) ? NaN : sqrt(csq);
}

inline double ExtEOS_VT::cT(double V, double T, const double *z)
{
    double csq = cT2(V, T, z);
    return (isnan(csq) || csq < 0) ? NaN : sqrt(csq);
}

inline double ExtEOS_VT::c(double V, double T)
{
    double csq = c2(V,T);
    return (isnan(csq) || csq < 0) ? NaN : sqrt(csq);
}
inline double ExtEOS_VT::cT(double V, double T)
{
    double csq = cT2(V,T);
    return (isnan(csq) || csq < 0) ? NaN : sqrt(csq);
}


/*******************************************
*** To do ***

// promote EOS_VT to ExtEOS_VT
class Hydro_VT : public ExtEOS_VT
{
protected:
    EOS_VT *hydro;
};

********************************************/

#endif // EOSLIB_EXT_EOS_VT_H
