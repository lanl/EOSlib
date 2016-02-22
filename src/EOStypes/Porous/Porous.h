#ifndef EOSLIB_POROUS_H
#define EOSLIB_POROUS_H

#include <PhiEq.h>
#include <EOS.h>
#include <OneDFunction.h>

#define PorousEOS_vers "PorousEOSv2.0.1"
#define PorousEOS_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *PorousEOSlib_vers;
extern const char *PorousEOSlib_date;

extern "C" { // DataBase functions
    char *EOS_FrozenPorous_Init();
    char *EOS_FrozenPorous_Info();
    void *EOS_FrozenPorous_Constructor();

    char *EOS_EqPorous_Init();
    char *EOS_EqPorous_Info();
    void *EOS_EqPorous_Constructor();

    char *EOS_DissipativePorous_Init();
    char *EOS_DissipativePorous_Info();
    void *EOS_DissipativePorous_Constructor();
}


//

class FrozenPorous;
class EqPorous;
class DissipativePorous;

//

class FrozenPorous : public EOS
{
    friend class FrozenPorousHugoniot;
    friend class FrozenPorousIsentrope;
    friend class FrozenPorousIsotherm;
private:
    EOS *solid;
    PhiEq *phi_eq;
    
    double phi_f;                               // volume fraction
    double B_f;                                 // compaction energy
    
    void operator=(const FrozenPorous&);        // disallowed
    FrozenPorous(const FrozenPorous&);          // disallowed
    FrozenPorous *operator &();                 // disallowed, use Duplicate()
protected:
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    FrozenPorous(EqPorous &porous, double phi1, double B1 = EOS::NaN,
                 const char *fpname = NULL);
    FrozenPorous() : EOS("FrozenPorous"),solid(NULL), phi_eq(NULL),
                     phi_f(1), B_f(0) {}
    void Reset(double phi1, double B1 = EOS::NaN);
    ~FrozenPorous();
 // EOS functions
    double     P(double V, double e);           // Pressure
    double     T(double V, double e);           // Temperature
    double     S(double V, double e);           // Entropy  
    double    c2(double V, double e);           // sound speed squared
    double Gamma(double V, double e);           // Gruneisen coefficient
    double   CV(double V, double e);            // Specific heat
    int NotInDomain(double V, double e);

    Hugoniot  *shock    (const HydroState &state);
    Isentrope *isentrope(const HydroState &state);
    Isotherm  *isotherm (const HydroState &state);
    
    ostream &PrintComponents(ostream &out);
 // extra functionality
    double     P(double V, double e, double phi1);
    double     T(double V, double e, double phi1);
    double     S(double V, double e, double phi1);
    double    c2(double V, double e, double phi1);
    double Gamma(double V, double e, double phi1);
    double   CV(double V, double e, double phi1);

    double V_ref_solid()                    { return solid->V_ref;        }
    double e_ref_solid()                    { return solid->e_ref;        }
    double     P_solid(double V, double e)  { return solid->P(V,e);       }
    double     T_solid(double V, double e)  { return solid->T(V,e);       }
    double     S_solid(double V, double e)  { return solid->S(V,e);       }
    double    c2_solid(double V, double e)  { return solid->c2(V,e);      }
    double Gamma_solid(double V, double e)  { return solid->Gamma(V,e);   }
    double   CV_solid(double V, double e)   { return solid->CV(V,e);     }

    double phi_0() const                    { return phi_eq->phi_0();     }
    double phi_1() const                    { return phi_eq->phi_1();     }
    double  PV_0() const                    { return phi_eq->PV_0();      }
    double  PV_1() const                    { return phi_eq->PV_1();      }

    double phi(double PV)                   { return phi_eq->phi(PV);     }
    double phi(WaveState &s)                { return phi_eq->phi(s.P*s.V);}
    double  PV(double phi)                  { return phi_eq->PV(phi);     }
    double energy(double phi)               { return phi_eq->energy(phi); }

    ostream &PrintSolid(ostream &out);
    ostream &PrintPhiEq(ostream &out);
};

inline double FrozenPorous::P(double V, double e, double phi1)
    { return phi1*solid->P(phi1*V,e-phi_eq->energy(phi1));       }
inline double FrozenPorous::T(double V, double e, double phi1)
    { return solid->T(phi1*V,e-phi_eq->energy(phi1));            }
inline double FrozenPorous::S(double V, double e, double phi1)
    { return solid->S(phi1*V,e-phi_eq->energy(phi1));            }
inline double FrozenPorous::c2(double V, double e, double phi1)
    { return solid->c2(phi1*V,e-phi_eq->energy(phi1));           }
inline double FrozenPorous::Gamma(double V, double e, double phi1)
    { return solid->Gamma(phi1*V,e-phi_eq->energy(phi1));        }
inline double FrozenPorous::CV(double V, double e, double phi1)
    { return solid->CV(phi1*V,e-phi_eq->energy(phi1));           }

inline ostream &FrozenPorous::PrintSolid(ostream &out)
{
    if( solid )
        solid->Print(out);
    return out;
}
inline ostream &FrozenPorous::PrintPhiEq(ostream &out)
{
    if( phi_eq )
        phi_eq->Print(out);
    return out;
}



class EqPorous : public EOS, protected OneDFunction
{
    friend class FrozenPorous;
    friend class IsothermEqPorous;
private:
    void operator=(const EqPorous&);            // disallowed
    EqPorous(const EqPorous&);                  // disallowed
    EqPorous *operator &();                     // disallowed, use Duplicate()
protected:
    EOS   *solid;
    PhiEq *phi_eq;
    
    double phi_max;
    double B_max;
    double dBdphi_max;
    
    virtual double f(double phi);               // OneDFunction::f
    double V_f, e_f, phi_f, B_f, P_f;           // for use with f
    int InitPhi();

    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
    EqPorous() : EOS("EqPorous"), solid(NULL), phi_eq(NULL) { }
    virtual ~EqPorous();
 // EOS functions
    virtual double     P(double V, double e);   // Pressure
    virtual double     T(double V, double e);   // Temperature
    virtual double     S(double V, double e);   // Entropy
    virtual double    c2(double V, double e);   // sound speed squared
    virtual double Gamma(double V, double e);   // Gruneisen coefficient
    virtual double    CV(double V, double e);   // Specific heat
    int NotInDomain(double V, double e);

    Isentrope *isentrope(const HydroState &state);
    Isotherm  *isotherm (const HydroState &state);
    
    ostream &PrintComponents(ostream &out);
 // extra functionality
    ostream &PrintSolid(ostream &out);
    ostream &PrintPhiEq(ostream &out);
    
    double phi_0() const                    { return phi_eq->phi_0();     }
    double phi_1() const                    { return phi_eq->phi_1();     }
    double  PV_0() const                    { return phi_eq->PV_0();      }
    double  PV_1() const                    { return phi_eq->PV_1();      }

    double    phi(double PV)                { return phi_eq->phi(PV);     }
    double    phi(WaveState &s)             { return phi_eq->phi(s.P*s.V);}
    double     PV(double phi)               { return phi_eq->PV(phi);     }
    double energy(double phi)               { return phi_eq->energy(phi); }
    
    double V_ref_solid()                    { return solid->V_ref;        }
    double e_ref_solid()                    { return solid->e_ref;        }
    double     P_solid(double V, double e)  { return solid->P(V,e);       }
    double     T_solid(double V, double e)  { return solid->T(V,e);       }
    double     S_solid(double V, double e)  { return solid->S(V,e);       }
    double    c2_solid(double V, double e)  { return solid->c2(V,e);      }
    double Gamma_solid(double V, double e)  { return solid->Gamma(V,e);   }
    double    CV_solid(double V, double e)  { return solid->CV(V,e);      }
 
    double     P(double V, double e, double phi1);
    double     T(double V, double e, double phi1);
    double     S(double V, double e, double phi1);
    double    c2(double V, double e, double phi1); // frozen sound speed
    double Gamma(double V, double e, double phi1); // frozen Gruneisen coef.
    double    CV(double V, double e, double phi1); // frozen specific heat
};

inline ostream &EqPorous::PrintSolid(ostream &out)
{
    if( solid )
        solid->Print(out);
    return out;
}
inline ostream &EqPorous::PrintPhiEq(ostream &out)
{
    if( phi_eq )
        phi_eq->Print(out);
    return out;
}

inline double EqPorous::P(double V, double e, double phi1)
    { return phi1*solid->P(phi1*V,e-phi_eq->energy(phi1));      }
inline double EqPorous::T(double V, double e, double phi1)
    { return solid->T(phi1*V,e-phi_eq->energy(phi1));           }
inline double EqPorous::S(double V, double e, double phi1)
    { return solid->S(phi1*V,e-phi_eq->energy(phi1));           }
inline double EqPorous::c2(double V, double e, double phi1)
    { return solid->c2(phi1*V,e-phi_eq->energy(phi1));          }
inline double EqPorous::Gamma(double V, double e, double phi1)
    { return solid->Gamma(phi1*V,e-phi_eq->energy(phi1));       }
inline double EqPorous::CV(double V, double e, double phi1)
    { return solid->CV(phi1*V,e-phi_eq->energy(phi1));          }

//
// Warning: DissipativePorous is NOT thermodynamically consistent
//        de != -PdV + TdS
//    Thermodynamic consistency requires introducing extra internal variable
//
class DissipativePorous : public EqPorous
{
    friend class DissipativeIsotherm;
private:
    void operator=(const DissipativePorous&);   // disallowed
    DissipativePorous(const DissipativePorous&);// disallowed
    DissipativePorous *operator &();            // disallowed, use Duplicate()
protected:
    double f(double phi);                       // OneDFunction::f
public:
    DissipativePorous();
    ~DissipativePorous();
 // EOS functions
    double     T(double V, double e);           // Temperature
    double     S(double V, double e);           // Entropy   
    double    c2(double V, double e);           // sound speed squared
    double Gamma(double V, double e);           // Gruneisen coefficient
    double   CV(double V, double e);            // Specific heat
    Isotherm *isotherm(const HydroState &state);
};

#endif // EOSLIB_POROUS_H
