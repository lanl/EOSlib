#ifndef EOSLIB_SESAME_EOS_H
#define EOSLIB_SESAME_EOS_H

#include <EOS_VT.h>
#include "Sesame_VT.h"


extern "C" { // DataBase functions
    char *EOS_Sesame_Init();
    char *EOS_Sesame_Info();
    void *EOS_Sesame_Constructor();
}
//
//
class Sesame_VT;
//
//
class Sesame : public EOS
{
    friend class Sesame_VT;
private:
    void operator=(const Sesame&);          // disallowed
    Sesame(const Sesame&);                  // disallowed
    Sesame *operator &();                   // disallowed, use Duplicate()
protected:
    char *sesnum;                           // not currently used
    char *VT_name;                          // name of Sesame_VT (optional)
    Sesame_VT *ses_VT;
    int interp;
    double de;                              // shift energy origin


    int rho_n;
    int e_n;
    double *rho_grd;
    // e_grd(rho,T) = e0(rho) + de(T)
    double *de_grd;
    double *e0_grd;
    double *P_table;
    double *T_table;

    double Vcache;
    double ecache;
    double Pcache[3];
    double Tcache[3];
    int Cache(double V, double e);
    
    EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);  // ToDo
    int InvertTable(double rho_ref);
public:
    Sesame();
    //Sesame(Sesame_VT &eos);
    ~Sesame();
    Sesame *Duplicate()    {return static_cast<Sesame*>(EOSbase::Duplicate());} 
    Sesame_VT *eosVT()     {return ses_VT ? ses_VT->Duplicate() : NULL;}
    
// EOS functions
    double P(double V, double e);           // Pressure
    double T(double V, double e);           // Temperature
    double S(double V, double e);           // Entropy
    
    double c2(double V, double e);          // sound speed squared
    double Gamma(double V, double e);       // Gruneisen coefficient
    double CV(double V, double e);          // Specific heat

    // int PT(double P, double T, HydroState &state);
    int NotInDomain(double V, double e);
};

#endif // EOSLIB_SESAME_EOS_H
