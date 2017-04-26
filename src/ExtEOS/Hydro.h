#ifndef EOSLIB_HYDRO_H
#define EOSLIB_HYDRO_H

#include "ExtEOS.h"
//
// promote EOS to ExtEOS
// 
class Hydro : public ExtEOS
{
private:
    void operator=(const Hydro&);               // disallowed
    Hydro();                                    // disallowed
    Hydro(const Hydro&);                        // disallowed
    Hydro *operator &();                        // disallowed, use Duplicate()
protected:
    EOS *hydro;
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
    //int  PreInit(Calc &);
    //int PostInit(Calc &, DataBase *);
public:
    Hydro(EOS &eos);
    ~Hydro();    
    double *z_f(double,double);
    ostream &PrintComponents(ostream&);
    Hydro *Duplicate() { return static_cast<Hydro*>(EOSbase::Duplicate());}
    ExtEOS *Copy();
    ExtEOS *Copy(const double *z);
    EOS *CopyEOS(const double *z);
 // EOS functions
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
    double  e_PV(double p, double V);
    int NotInDomain(double V, double e);
    Isentrope   *isentrope(const HydroState &state);
    Hugoniot        *shock(const HydroState &state);
    Isotherm     *isotherm(const HydroState &state);
    Detonation *detonation(const HydroState &state, double p0);
    int PT(double P, double T, HydroState &state);
 // ExtEOS functions
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
    double  e_PV(double p, double V, const double *z);
    int NotInDomain(double V, double e, const double *z);
    Isentrope *isentrope(const HydroState &state, const double*);
    Hugoniot      *shock(const HydroState &state, const double*);
    Isotherm   *isotherm(const HydroState &state, const double*);
 // 
    int Rate(double V, double e, const double *z, double *zdot);
    int TimeStep(double V, double e, const double *z, double &dt);
    int Integrate(double V, double e, double *z, double dt);
    int Equilibrate(double V, double e, double *z);
 // Hydro functions
    std::ostream &PrintHydro(std::ostream &out);   
};

inline std::ostream &Hydro::PrintHydro(std::ostream &out)
{
    if( hydro )
        hydro->Print(out);
    return out;
}

#endif // EOSLIB_HYDRO_H
