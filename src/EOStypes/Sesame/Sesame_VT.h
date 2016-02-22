#ifndef EOSLIB_SESAME_VT_H
#define EOSLIB_SESAME_VT_H

#define Sesame_VT_vers "Sesame_VTv2.0.1"
#define Sesame_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *Sesame_VTlib_vers;
extern const char *Sesame_VTlib_date;

extern "C" { // DataBase functions
    char *EOS_VT_Sesame_Init();
    char *EOS_VT_Sesame_Info();
    void *EOS_VT_Sesame_Constructor();
}
//
//
class Sesame_VT : public EOS_VT
{
    friend class Sesame;
public:
    static char *DATA;
private:
    void operator=(const Sesame_VT&);            // disallowed
    Sesame_VT(const Sesame_VT&);                 // disallowed
    Sesame_VT *operator &();                     // disallowed, use Duplicate()
protected:
    char *sesnum;
    int interp;
public: //debugging
    int rho_n;
    int T_n;
    double *rho_grd;
    double *T_grd;
    double *P_table;
    double *e_table;
    double *S_table;
protected:
    int ReadArray(const char *file, double *array, int n);

    double Vcache;
    double Tcache;
    double Pcache[3];
    double ecache[3];
    int Cache(double V, double T);
    void ComputeS();
    void Shift_e(double de);
    
    // EOS *Ve();
    int SesDir(DataBase &db);    
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
public:
    Sesame_VT();
    ~Sesame_VT();
    Sesame_VT *Duplicate()    {return static_cast<Sesame_VT*>(EOSbase::Duplicate());} 
 // EOS functions
    int NotInDomain(double V, double T);
    double P(double V, double T);               // Pressure
    double e(double V, double T);               // specific energy
    double S(double V, double T);               // entropy
    double P_V(double V, double T);             // dP/dV @ const T
    double P_T(double V, double T);             // dP/dT @ const V
    double CV(double V, double T);              // T*(d/dT)S = de/dT
/***
    //
    double F(double V, double T);               // F = e - T*S
    double CP(double V, double T);              // Specific heat at constant P
    double KS(double V, double T);              // Isentropic bulk modulus
    double KT(double V, double T);              // Isothermal bulk modulus
    double Gamma(double V, double T);           // -(V/T)dT/dV @ const S
    double beta(double V, double T);            // volumetric thermal expansion
    double c2(double V, double T);              // isothermal (sound speed)^2
    double cT2(double V, double T);             // isothermal (sound speed)^2
***/
 // Extra functionality
    double V(double p, double T);  
};

#endif // _SESAME_EOS_H

