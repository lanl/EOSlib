#ifndef EOSLIB_EXT_EOS_H
#define EOSLIB_EXT_EOS_H

#include <EOS.h>
#include "IDOF.h"
//
// base class for Extended EOS
//      EOS with internal degrees of freedom (IDOF)
//      and rate equations for evolution of the IDOF
class ExtEOS : public EOS
{
private:
    void operator=(const ExtEOS&);       // disallowed
    ExtEOS *operator &();                // disallowed, use Duplicate()
protected:
    IDOF *z_ref;    // internal variables in ref state
    int n;          // = z_ref.N(), number of internal degrees of freedom
    double *ztmp;   // temporary storage for z_f()
    int frozen;     // flag for frozen or equilibrium z_f()
    int Replace(const double *z);                   // z_ref.z[i] = z[i]
    ExtEOS(const char *eos_type);    
    ExtEOS(const ExtEOS&);    
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    virtual void PrintParams(std::ostream &out);
    virtual int ConvertParams(Convert &convert);    
    virtual int  PreInit(Calc &);
    virtual int PostInit(Calc &, DataBase *);
public:
    int N() const       { return n;         }
    IDOF *Zref()        { return z_ref;     }       // return IDOF & ?
    virtual int set_z(const char *var, double value, double *z);
    virtual int get_z(double *z, const char *var, double &value);
    // return value:
    //  -1, error
    //   0, var known and operation successful
    //   1, var not known
    int set_zref(const char *var, double value);
    int get_zref(const char *var, double &value);
    //
    double *Z()         { return ztmp;      }       // last z_f, return double const * ?
    double &Z(int i)    { return ztmp[i];   }       // last z_f
    void Equilibrium()                  { frozen = 0;}
    int Frozen(const double *z=NULL)    { frozen = 1; return Replace(z);}
    int IsFrozen() const { return frozen;}
    virtual double *z_f(double V, double e);        // frozen or equilibrium z
    // returns ztmp, z_f or ztmp set by Equilibrate
    virtual ~ExtEOS();
    ExtEOS *Duplicate() { return static_cast<ExtEOS*>(EOSbase::Duplicate());}
    virtual ExtEOS *Copy() = 0;             // derived class copy constructor
    virtual EOS *CopyEOS(const double *z);  // used by RiemannSolver_genX
    virtual ExtEOS *Copy(const double *z);
    //EqExtEOS *EqEOS()          {return new EqExtEOS(*this);   }
    //EqExtEOS *EqEOS(double *z) {return new EqExtEOS(*this,z); }
    // ostream &PrintComponents(ostream &out);
 // EOS functions, z = z_f(V,e)
    virtual double     P(double V, double e);
    virtual double     T(double V, double e);
    virtual double     S(double V, double e);
    virtual double    c2(double V, double e);
    virtual double Gamma(double V, double e);
    virtual double    CV(double V, double e);
    virtual double    CP(double V, double e);
    virtual double    KT(double V, double e);
    virtual double  beta(double V, double e);
    virtual double    FD(double V, double e);
    virtual double  e_PV(double p, double V);
    virtual int NotInDomain(double V, double e);
    // using EOS::c;
    // using EOS::KS;
    double KS(double V, double e);              // Isentropic bulk modulus
    double  c(double V, double e);              // sound speed = sqrt(c2)
 // Extended functionality
    virtual double     P(double V, double e, const double *z) = 0;
    virtual double     T(double V, double e, const double *z);
    virtual double     S(double V, double e, const double *z);
    virtual double    c2(double V, double e, const double *z);
    virtual double Gamma(double V, double e, const double *z);
    virtual double    CV(double V, double e, const double *z);
    virtual double    CP(double V, double e, const double *z);
    virtual double    KT(double V, double e, const double *z);
    virtual double  beta(double V, double e, const double *z);
    virtual double    FD(double V, double e, const double *z);
    virtual double  e_PV(double p, double V, const double *z);
            double    KS(double V, double e, const double *z);
            double     c(double V, double e, const double *z);
    virtual int NotInDomain(double V, double e, const double *z);
    virtual Isentrope *isentrope(const HydroState &state, const double *zi);
    virtual Hugoniot      *shock(const HydroState &state, const double *zi);
    virtual Isotherm   *isotherm(const HydroState &state, const double *zi);
    virtual Detonation     *detonation(const HydroState &state, double P0, const double *zi);
    virtual Deflagration *deflagration(const HydroState &state, double P0, const double *zi);
    // functions of (const HydroState &state)
    using EOS::isentrope;
    using EOS::shock;
    using EOS::isotherm;
    // functions of (const HydroState &state, double P0)
    using EOS::detonation;
    using EOS::deflagration;
 // 
    virtual int  Rate(double V, double e, const double *z, double *zdot);
    // -1, on error
    //  ExtEOS::n, otherwise (dimension of z)
    virtual int TimeStep(double V, double e, const double *z, double &dt);
    // -1, error
    //  0, no constraint
    //  1, dt = time_step for Rate
    //  2, use (stiff) integrate to avoid small time step
    //  3, Integrate() available
    virtual int Integrate(double V, double e, double *z, double dt);
    // Single (asymptotic) time step
    virtual int Equilibrate(double V, double e, double *z);
    // set z to equilibrium value, ie, Rate = 0
    virtual int ddz(int i, double V, double e, const double *z,
                           double &dP, double &dT);
    // return value:
    //  -1, error calculating dP or dT or i>=ExtEOS::n
    //   0, dP = (d/dz[i])P and dT = (d/dz[i])T
    //   1, not available (default, ToDo: use finite difference)
    virtual int var(const char *name, double V, double e, const double *z,
                        double &value);
    // return value:
    //  -1, error calculating name
    //   0, value = name(V,e,z) where name is known to derived class
    //   1, no such variable
};
//
inline int deleteExtEOS(ExtEOS *&eos)
{
    int status = deleteEOSbase(eos,"ExtEOS");
    eos = NULL;
    return status;
}
//
inline ostream & operator<<(ostream &out, ExtEOS &eos)
            { return eos.Print(out); }
//
inline int ExtEOS::set_zref(const char *var, double value)
{
    return z_ref ? set_z(var, value, &(*z_ref)) : 1;
}
inline int ExtEOS::get_zref(const char *var, double &value)
{
    return z_ref ? get_z(&(*z_ref), var, value) : 1;
}
//
inline double ExtEOS::c(double V, double e)
{
    double csq = c2(V,e);
    return (isnan(csq) || csq < 0) ? NaN : sqrt(csq);
}
inline double ExtEOS::KS(double V, double e)
{
    return c2(V,e)/V;
}
inline double ExtEOS::c(double V, double e, const double *z)
{
    double csq = c2(V, e, z);
    return (isnan(csq) || csq < 0) ? EOS::NaN : sqrt(csq);
}
inline double ExtEOS::KS(double V, double e, const double *z)
{
    return c2(V, e, z)/V;
}
//
// Database
// 
ExtEOS *FetchExtEOS(const char *type, const char *name, DataBase &db);
ExtEOS *FetchNewExtEOS(const char *type, const char *name, DataBase &db);
ExtEOS *FetchExtEOS(const char *type_name, DataBase &db);
ExtEOS *FetchNewExtEOS(const char *type_name, DataBase &db);

#endif // EOSLIB_EXT_EOS_H
