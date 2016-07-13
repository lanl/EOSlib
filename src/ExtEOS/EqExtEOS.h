#ifndef EOSLIB_EQEXT_EOS_H
#define EOSLIB_EQEXT_EOS_H

#include <ExtEOS.h>

extern "C" { // DataBase functions
    char *EOS_EqExtEOS_Init();
    char *EOS_EqExtEOS_Info();
    void *EOS_EqExtEOS_Constructor();
}
//
//  DEPRICATED: use ExtEOS with frozen=0
//
class EqExtEOS : public ExtEOS
{
private:
    void operator=(const EqExtEOS&);            // disallowed
    EqExtEOS *operator &();                     // disallowed, use Duplicate()
protected:
    ExtEOS *exteos;
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
    int  PreInit(Calc &);
    int PostInit(Calc &, DataBase *);
public:
    IDOF *Zref() {return exteos ? exteos->Zref() : NULL;}
    double *z_f(double V, double e);            // equilibrium z
    EqExtEOS();
    EqExtEOS(ExtEOS&);    
    EqExtEOS(ExtEOS&, const double *z);    
    ~EqExtEOS();
    EqExtEOS *Duplicate() {return static_cast<EqExtEOS*>(EOSbase::Duplicate());}
    ExtEOS   *Copy();
    ExtEOS *Ext_EOS() { return exteos ? exteos->Duplicate(): NULL; }
    ostream &PrintComponents(ostream &out);
 // EOS functions, z = z_f(V,e)
    double     P(double V, double e);
    double     T(double V, double e);
    double     S(double V, double e);
    double    c2(double V, double e);
    double Gamma(double V, double e);
    double    CV(double V, double e);
    double    CP(double V, double e);
    double    KT(double V, double e);
    double  beta(double V, double e);
    double    FD(double V, double e);
    int NotInDomain(double V, double e);
    double KS(double V, double e);              // Isentropic bulk modulus
    double  c(double V, double e);              // sound speed = sqrt(c2)
 // Extended functionality
    double     P(double V, double e, const double *z);
    double     T(double V, double e, const double *z);
    double     S(double V, double e, const double *z);
    double    c2(double V, double e, const double *z);
    double Gamma(double V, double e, const double *z);
    double    CV(double V, double e, const double *z);
    double    CP(double V, double e, const double *z);
    double    KT(double V, double e, const double *z);
    double  beta(double V, double e, const double *z);
    double    FD(double V, double e, const double *z);
    double    KS(double V, double e, const double *z);
    double     c(double V, double e, const double *z);
    int NotInDomain(double V, double e, const double *z);
 // 
    int        Rate(double V, double e, const double *z, double *zdot);
    int    TimeStep(double V, double e, const double *z, double &dt);
    int   Integrate(double V, double e,       double *z, double dt);
    int Equilibrate(double V, double e,       double *z);
    int var(const char *name, double V, double e, const double *z,
                double &value);
};


inline double EqExtEOS::c(double V, double e)
{
    double csq = c2(V,e);
    return (std::isnan(csq) || csq < 0) ? NaN : sqrt(csq);
}
inline double EqExtEOS::KS(double V, double e)
{
    return c2(V,e)/V;
}
inline double EqExtEOS::c(double V, double e, const double *z)
{
    double csq = c2(V, e, z);
    return (std::isnan(csq) || csq < 0) ? EOS::NaN : sqrt(csq);
}
inline double EqExtEOS::KS(double V, double e, const double *z)
{
    return c2(V, e, z)/V;
}


#endif // EOSLIB_EQEXT_EOS_H
