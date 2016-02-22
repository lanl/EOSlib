#ifndef EOSLIB_ELASTIC_H
#define EOSLIB_ELASTIC_H
//
#include <ExtEOS_VT.h>
#include <ExtEOS.h>
//
#define Elastic_VT_vers "Elasticv2.0.1"
#define Elastic_VT_date "Jan. 5, 2013"
//
extern const char *Elastic_VTlib_vers;
extern const char *Elastic_VTlib_date;
//
// elastic strain tensor = (1/3)*log(V/V_ref)*I + (eps_el/3)*diag[2,-1,-1]
// plastic strain tensor = (eps_pl/3)*diag[2,-1,-1]
// With eps_pl = log(V/V_ref) - eps_el
// total strain tensor = log(V/V_ref)*diag[1,0,0] is uniaxial
// elastic (shear) strain, z[0] = eps_el
class ElasticStrain : public IDOF
{ // n = 1 and z[0] = eps_el
public:
    static const char *dname;
    static const char *var_dname[];              // name of z[i]    
    ElasticStrain();
    ElasticStrain(const ElasticStrain &);
    ~ElasticStrain();
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
// abstract base class for Elastic ExtEOS_VT
// F = F_hydro(V,T) + F_shear(V,T,eps_el)
class Elastic_VT : public ExtEOS_VT,  protected ElasticStrain
{
private:
    Elastic_VT();                               // disallowed
    Elastic_VT *operator &();                   // disallowed, use Duplicate()
    void operator=(const Elastic_VT&);          // disallowed
public:
 // misc
    virtual ostream &PrintComponents(ostream &out);
    virtual ~Elastic_VT(); 
    Elastic_VT *Duplicate()     { return (Elastic_VT*)EOSbase::Duplicate();}  
    virtual ExtEOS_VT *Duplicate(const double *z);
    IDOF *z_ref();
    double *z_f(double V, double T);
    int Rate(double V, double T, const double *z, double *zdot);
    EOS_VT *Hydro();
    EOS    *Hydro_Ve();
protected:
    EOS_VT *hydro;
    Elastic_VT(const char *ptype);
    Elastic_VT(const Elastic_VT&);
    virtual int InitParams(Parameters &, Calc &, DataBase *db);
    // using ElasticStrain::InitParams;
    int InitParams(Calc &calc) {return ElasticStrain::InitParams(calc); }
    virtual void PrintParams(ostream &out);
    virtual int ConvertParams(Convert &convert);    
    virtual int PreInit(Calc &);
    virtual int PostInit();
public:
    double eps_el(double V); // elastic shear strain
 // stress positive in tension, eps_el positive in expansion
    virtual double Fshear(double V, double T, double eps_el)   = 0;
    virtual double Sshear(double V, double T, double eps_el)   = 0;
 // eshear = Fshear + T*Sshear
    virtual double eshear(double V, double T, double eps_el);
    virtual double Pshear(double V, double T, double eps_el)   = 0;
    virtual double  shear(double V, double T, double eps_el)   = 0;
 // dshear = (d/deps_el) shear
    virtual double dshear(double V, double T, double eps_el)   = 0;
 // Cauchy stress positive in tension (opposite to hydro-static pressure)
 // stress (xx component) =  Pshear + (2/3)*shear
    virtual double   stress(double V, double T, double eps_el);
 // stress_V = (d/dV) stress
    virtual double stress_V(double V, double T, double eps_el) = 0;
 // stress_T = (d/dT) stress
    virtual double stress_T(double V, double T, double eps_el) = 0;
 // CVshear = (d/dT) eshear
    virtual double  CVshear(double V, double T, double eps_el) = 0;
 // ExtEOS_VT functions
 // P = hydro->P - stress
    virtual double     P(double V, double T, const double *z);
    virtual double     e(double V, double T, const double *z);
    virtual double     S(double V, double T, const double *z);
    virtual double   P_V(double V, double T, const double *z);
    virtual double   P_T(double V, double T, const double *z);
    virtual double    CV(double V, double T, const double *z);
    virtual double     F(double V, double T, const double *z);    // F = e - T*S
    virtual int NotInDomain(double V, double T, const double *z);
    //using ExtEOS_VT::P;
    //using ExtEOS_VT::e;
    //using ExtEOS_VT::S;
    //using ExtEOS_VT::P_V;
    //using ExtEOS_VT::P_T;
    //using ExtEOS_VT::CV;
    //using ExtEOS_VT::F;
    //using ExtEOS_VT::NotInDomain;
    double     P(double V, double T) { return ExtEOS_VT::P(V,T);    }
    double     e(double V, double T) { return ExtEOS_VT::e(V,T);    }
    double     S(double V, double T) { return ExtEOS_VT::S(V,T);    }
    double   P_V(double V, double T) { return ExtEOS_VT::P_V(V,T);  }
    double   P_T(double V, double T) { return ExtEOS_VT::P_T(V,T);  }
    double    CV(double V, double T) { return ExtEOS_VT::CV(V,T);   }
    double     F(double V, double T) { return ExtEOS_VT::F(V,T);    }
    int NotInDomain(double V, double T) { return ExtEOS_VT::NotInDomain(V,T); }
    //
    double Phydro(double V, double T);
    double Shydro(double V, double T);
    double ehydro(double V, double T);
};
//
inline int deleteElastic_VT(Elastic_VT *&eos)
{
    int status = deleteEOSbase(eos,"Elastic_VT");
    eos = NULL;    
    return status;
}
//
inline double Elastic_VT::eps_el(double V)
    { return log(V/V_ref) + z[0];                   }
inline EOS_VT *Elastic_VT::Hydro()
    { return hydro ? hydro->Duplicate() : NULL;     }
inline EOS *Elastic_VT::Hydro_Ve()
    { return hydro ? hydro->eosVe() : NULL;         }
inline double Elastic_VT::Phydro(double V, double T)
{
    return hydro ? hydro->P(V,T) : NaN;
}
inline double Elastic_VT::Shydro(double V, double T)
{
    return hydro ? hydro->S(V,T) : NaN;
}
inline double Elastic_VT::ehydro(double V, double T)
{
    return hydro ? hydro->e(V,T) : NaN;
}

// promote to ExtEOS

extern "C" { // DataBase functions
    char *EOS_ElasticEOS_Init();
    char *EOS_ElasticEOS_Info();
    void *EOS_ElasticEOS_Constructor();
}
class ElasticEOS : public ExtEOS
{
private:
    void operator=(const ElasticEOS&);          // disallowed
    ElasticEOS *operator &();                   // disallowed, use Duplicate()
public:
 // misc
    ElasticEOS();
    ElasticEOS(Elastic_VT&);
    virtual ~ElasticEOS();
    EOS_VT *eosVT();
    ExtEOS_VT *exteosVT();
    ElasticEOS *Duplicate();
    ExtEOS *Copy();
    ExtEOS *Copy(const double *z);
    virtual ostream &PrintComponents(ostream &out);
    //virtual double *z_f(double V, double e);      // ExtEOS
protected:
    Elastic_VT *elastic;
    EOS *hydro;
    // IDOF *z_ref;                                 // ExtEOS
 //
    ElasticEOS(const char *ptype);
    ElasticEOS(ElasticEOS&);
    ElasticEOS(ElasticEOS&, const double *z);
    virtual void PrintParams(ostream &out);
    virtual int ConvertParams(Convert &convert);    
    virtual int  PreInit(Calc &);
    virtual int PostInit(Calc &, DataBase *);
public:
    double eps_el(double V); // elastic shear strain
 // stress positive in tension, eps_el positive in expansion
    double Fshear(double V, double e, double eps_el);
    double Sshear(double V, double e, double eps_el);
 // eshear = Fshear + T*Sshear
    double eshear(double V, double e, double eps_el);
    double Pshear(double V, double e, double eps_el);
    double  shear(double V, double e, double eps_el);
    double dshear(double V, double e, double eps_el);
 // Cauchy stress positive in tension (opposite to hydro-static pressure)
 // stress (xx component) =  Pshear + (2/3)*shear
    double stress  (double V, double e, double eps_el);
 // stress_V = (d/dV) stress
    double stress_V(double V, double e, double eps_el);
 // stress_T = (d/dT) stress
    double stress_T(double V, double e, double eps_el);
 // CVshear = (d/dT) eshear
    double  CVshear(double V, double e, double eps_el);
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
    virtual int Rate(double V, double e, const double *z, double *zdot);
    // using ExtEOS::P;
    // using ExtEOS::T;
    // using ExtEOS::S;
    // using ExtEOS::c2;
    // using ExtEOS::Gamma;
    // using ExtEOS::CV;
    // using ExtEOS::CP;
    // using ExtEOS::KT;
    // using ExtEOS::beta;
    // using ExtEOS::NotInDomain;
    double     P(double V, double e) { return ExtEOS::P(V,e);       }
    double     T(double V, double e) { return ExtEOS::T(V,e);       }
    double     S(double V, double e) { return ExtEOS::S(V,e);       }
    double    c2(double V, double e) { return ExtEOS::c2(V,e);      }
    double Gamma(double V, double e) { return ExtEOS::Gamma(V,e);   }
    double    CV(double V, double e) { return ExtEOS::CV(V,e);      }
    double    CP(double V, double e) { return ExtEOS::CP(V,e);      }
    double    KT(double V, double e) { return ExtEOS::KT(V,e);      }
    double  beta(double V, double e) { return ExtEOS::beta(V,e);    }
    int NotInDomain(double V, double e) { return ExtEOS::NotInDomain(V,e); }
    double Phydro(double V, double e);
};

inline double ElasticEOS::Phydro(double V, double e)
    { return hydro ? hydro->P(V,e) : NaN;           }

inline EOS_VT *ElasticEOS::eosVT()
    { return elastic ? elastic->Duplicate() : NULL; }
inline ExtEOS_VT *ElasticEOS::exteosVT()
    { return elastic ? static_cast<ExtEOS_VT*>(elastic->Duplicate()) : NULL; }
inline ElasticEOS *ElasticEOS::Duplicate()
    { return static_cast<ElasticEOS*>(EOSbase::Duplicate());    }
inline double ElasticEOS::eps_el(double V)
    { return elastic ? elastic->eps_el(V) : NaN;                }
inline double ElasticEOS::Fshear(double V, double e, double epsel)
{
    ztmp[0] = epsel;
    double Tve = T(V,e, ztmp);
    return isnan(Tve) ? NaN : elastic->Fshear(V,Tve, ztmp[0]);    
}
inline double ElasticEOS::Sshear(double V, double e, double epsel)
{
    ztmp[0] = epsel;
    double Tve = T(V,e, ztmp);
    return isnan(Tve) ? NaN : elastic->Sshear(V,Tve, ztmp[0]);    
}
inline double ElasticEOS::eshear(double V, double e, double epsel)
{
    ztmp[0] = epsel;
    double Tve = T(V,e, ztmp);
    return isnan(Tve) ? NaN : elastic->eshear(V,Tve, ztmp[0]);    
}
inline double ElasticEOS::Pshear(double V, double e, double epsel)
{
    ztmp[0] = epsel;
    double Tve = T(V,e, ztmp);
    return isnan(Tve) ? NaN : elastic->Pshear(V,Tve, ztmp[0]);    
}
inline double ElasticEOS::shear(double V, double e, double epsel)
{
    ztmp[0] = epsel;
    double Tve = T(V,e, ztmp);
    return isnan(Tve) ? NaN : elastic->shear(V,Tve, ztmp[0]);    
}
inline double ElasticEOS::dshear(double V, double e, double epsel)
{
    ztmp[0] = epsel;
    double Tve = T(V,e, ztmp);
    return isnan(Tve) ? NaN : elastic->dshear(V,Tve, ztmp[0]);    
}
inline double ElasticEOS::stress(double V, double e, double epsel)
{
    ztmp[0] = epsel;
    double Tve = T(V,e, ztmp);
    return isnan(Tve) ? NaN : elastic->stress(V,Tve, ztmp[0]);    
}
inline double ElasticEOS::stress_V(double V, double e, double epsel)
{
    ztmp[0] = epsel;
    double Tve = T(V,e, ztmp);
    return isnan(Tve) ? NaN : elastic->stress_V(V,Tve, ztmp[0]);    
}
inline double ElasticEOS::stress_T(double V, double e, double epsel)
{
    ztmp[0] = epsel;
    double Tve = T(V,e, ztmp);
    return isnan(Tve) ? NaN : elastic->stress_T(V,Tve, ztmp[0]);    
}
inline double ElasticEOS::CVshear(double V, double e, double epsel)
{
    ztmp[0] = epsel;
    double Tve = T(V,e, ztmp);
    return isnan(Tve) ? NaN : elastic->CVshear(V,Tve, ztmp[0]);    
}   

#endif // EOSLIB_ELASTIC_H
