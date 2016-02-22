#ifndef EOSLIB_ELASTIC_PLASTIC_H
#define EOSLIB_ELASTIC_PLASTIC_H

#include "Elastic.h"
#include <ExtEOS.h>
//
#define ElasticPlastic_vers "Elasticv2.0.1"
#define ElasticPlastic_date "Jan. 5, 2013"
//
extern const char *ElasticPlastic_lib_vers;
extern const char *ElasticPlastic_lib_date;
//
// plastic strain tensor = (eps_pl/3)*diag[2,-1,-1]
// total   strain tensor = log(V/V_ref)*diag[1, 0, 0]
// elastic strain tensor = total_strain - plastic_strain
//                       = (1/3)*log(V/V_ref)*I + (eps_el/3)*diag[2,-1,-1]
// eps_el = log(V/V_ref) - eps_pl
// 
class PlasticStrain : public IDOF
{ // n = 1 and z[0] = eps_pl
public:
    static const char *dname;
    static const char *var_dname[];              // name of z[i]    
    PlasticStrain();
    PlasticStrain(const PlasticStrain &);
    ~PlasticStrain();
    IDOF *Copy();
 //
    int InitParams(Calc &);
    int ConvertParams(Convert &);
    void PrintParams(ostream &);
 //
    ostream &Print(ostream &, const double *);
    void Load(Calc &, double *);
};
//
// abstract base class for ElasticPlastic ExtEOS
class ElasticPlastic : public ExtEOS
{
private:
    void operator=(const ElasticPlastic&);       // disallowed
    ElasticPlastic *operator &();                // disallowed, use Duplicate()
public:
 // misc
    virtual ostream &PrintComponents(ostream &out);
    virtual ~ElasticPlastic();
    ElasticPlastic *Duplicate();
    //virtual ExtEOS *Copy();
    //double   *z_f(double V, double e);                // ExtEOS
    double eps_el(double V, double e, const double *z); // elastic shear strain
protected:
    //IDOF *z_ref;                                      // ExtEOS
    Elastic_VT *elastic;
    EOS *hydro;
    double *z_el;                   // elastic strain for internal use
 //
    ElasticPlastic(const char *ptype);
    ElasticPlastic(const ElasticPlastic&);
    virtual void PrintParams(ostream &out);
    virtual int ConvertParams(Convert &convert);    
    virtual int PreInit(Calc &);
    virtual int PostInit(Calc &, DataBase *db);
public:
 // plastic flow when yield_func > yields_surf
    virtual double yield_func(double V, double e, const double *z)          = 0;
    virtual double yield_surf(double V, double e, const double *z)          = 0;
    virtual    int  Rate(double V, double e, const double *z, double *zdot) = 0;
    virtual int TimeStep(double V, double e, const double *z, double &dt)   = 0;
    // int  Integrate(double V, double e, double *z, double dt);
    // int  Equilibrate(double V, double e, double *z);
 // ExtEOS functions
    virtual double     P(double V, double e, const double *z);
    virtual double     T(double V, double e, const double *z);
    virtual double     S(double V, double e, const double *z);
    virtual double    c2(double V, double e, const double *z);
    virtual double Gamma(double V, double e, const double *z);
    virtual double    CV(double V, double e, const double *z);
    virtual double    CP(double V, double e, const double *z);
    virtual double    KT(double V, double e, const double *z);
    virtual double  beta(double V, double e, const double *z);
    virtual int NotInDomain(double V, double e, const double *z);
 // Elastic functions
    double   Fshear(double V, double e, const double *z);
    double   Sshear(double V, double e, const double *z);
    double   eshear(double V, double e, const double *z);
    double   Pshear(double V, double e, const double *z);
    double    shear(double V, double e, const double *z);
    double stress  (double V, double e, const double *z);
    double stress_V(double V, double e, const double *z);
    double stress_T(double V, double e, const double *z);
    double  CVshear(double V, double e, const double *z);
 // Hydro functions
    double Phydro(double V, double e, const double *z);
    double Shydro(double V, double e, const double *z);
    double ehydro(double V, double e, const double *z);
    //
    int var(const char *name, double V, double e, const double *z,
                double &value);   
};
//
inline ElasticPlastic *ElasticPlastic::Duplicate()
    { return (ElasticPlastic*)EOSbase::Duplicate();         }  
inline double ElasticPlastic::eps_el(double V, double e, const double *z)
    { return elastic ? elastic->eps_el(V) - z[0] : NaN;   }

inline double ElasticPlastic::Fshear(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return isnan(Tve) ? NaN : elastic->Fshear(V, Tve, z_el[0]);   
}
inline double ElasticPlastic::Sshear(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return isnan(Tve) ? NaN : elastic->Sshear(V, Tve, z_el[0]);   
}
inline double ElasticPlastic::eshear(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return isnan(Tve) ? NaN : elastic->eshear(V, Tve, z_el[0]);   
}
inline double ElasticPlastic::Pshear(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return isnan(Tve) ? NaN : elastic->Pshear(V, Tve, z_el[0]);   
}
inline double ElasticPlastic::shear(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return isnan(Tve) ? NaN : elastic->shear(V, Tve, z_el[0]);   
}
inline double ElasticPlastic::stress  (double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return isnan(Tve) ? NaN : elastic->stress(V, Tve, z_el[0]);   
}
inline double ElasticPlastic::stress_V(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return isnan(Tve) ? NaN : elastic->stress_V(V, Tve, z_el[0]);   
}
inline double ElasticPlastic::stress_T(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return isnan(Tve) ? NaN : elastic->stress_T(V, Tve, z_el[0]);   
}
inline double ElasticPlastic::CVshear(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return isnan(Tve) ? NaN : elastic->CVshear(V, Tve, z_el[0]);   
}
    
inline double ElasticPlastic::Phydro(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return isnan(Tve) ? NaN : elastic->Phydro(V, Tve);   
}
inline double ElasticPlastic::Shydro(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return isnan(Tve) ? NaN : elastic->Shydro(V, Tve);   
}
inline double ElasticPlastic::ehydro(double V, double e, const double *z)
{
    double Tve = T(V,e, z);
    return isnan(Tve) ? NaN : elastic->ehydro(V, Tve);   
}
    
#endif // EOSLIB_ELASTIC_PLASTIC_H
