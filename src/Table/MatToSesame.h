#ifndef EOSLIB_MAT_TO_SESAME_H
#define EOSLIB_MAT_TO_SESAME_H

#include "Table.h"
#include "EOS.h"
#include "EOS_VT.h"
//
//
#define MatToSesame_vers "MatToSesame_v2.0.1"
#define MatToSesame_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"
//
extern const char *MatToSesame_lib_vers;
extern const char *MatToSesame_lib_date;
//
extern "C" { // DataBase functions
    char *Table_MatToSesame_Init();
    char *Table_MatToSesame_Info();
    void *Table_MatToSesame_Constructor();
}
//
class MatToSesame : public Table
{
    MatToSesame(const MatToSesame&);            // disallowed    
    void operator=(const MatToSesame&);         // disallowed
    MatToSesame *operator &();                  // disallowed, use Duplicate()
protected:
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);
    int AsciiOut(double *rho, double *T, double *P, double *e, double *F,
                 FILE *fh);
public:
    MatToSesame();
    ~MatToSesame();
    ostream &PrintComponents(ostream &out);
    //
    int AsciiTable(const char *file);
    //
    char   *mat;    // EOSlib material
    EOS    *eos;
    EOS_VT *VTeos;
    //
    int ID;         // table number
    char *cdate;    // creation date
    int rev;        // revision number
    // 201 table parameters
    double Zbar;    // mean atomic number
    double Abar;    // mean atomic mass
    double rho0;    // Normal density
    double B0;      // Bulk modulus (isothermal at T=0 or T_min or Tref ?)
    double cex;     // exchange coefficient (dimensions ?)
    // 301 table: P(nr,nT) and T(nr,nT)
    int nr;         // number of densities
    double rho_min; // minimum rho
    double rho_max; // maximum rho
    int nT;         // number of temperatures
    double T_min;   // minimum T
    double T_max;   // maximum T
    //
    char *Date();   // current date (mmddyy)
};

#endif // EOSLIB_MAT_TO_SESAME_H

