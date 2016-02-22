#ifndef EOSLIB_BIRCH_MURNAGHAN_EOS_H
#define EOSLIB_BIRCH_MURNAGHAN_EOS_H

#include "GenHayes.h"
#include "BirchMurnaghanParams.h"

#define BirchMurnaghan_vers "BirchMurnaghanv2.0.1"
#define BirchMurnaghan_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *BirchMurnaghanlib_vers;
extern const char *BirchMurnaghanlib_date;

extern "C" { // DataBase functions
    char *EOS_BirchMurnaghan_Init();
    char *EOS_BirchMurnaghan_Info();
    void *EOS_BirchMurnaghan_Constructor();
}
//
//
class EOS_VT;
class BirchMurnaghan_VT;
//
//  GenHayes form of EOS with
//    BirchMurnaghan cold curve
//    Specific heat is function of single scaled Debye temperature
//      Cv(V,T) = SCv(V/theta(V))
//    Gruneisen coefficient is function of only V
//      Gamma(V) = a + b*V/V0 = - dln[theta(V)]/dln(V)
//
class BirchMurnaghan : public GenHayes, public BirchMurnaghanBase
{
    friend class BirchMurnaghan_VT;
private:
	void operator=(const BirchMurnaghan&);	// disallowed
	BirchMurnaghan(const BirchMurnaghan&);  // disallowed
	BirchMurnaghan *operator &();			// disallowed, use Duplicate()
protected:
	BirchMurnaghan(BirchMurnaghan_VT &eos);
    EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
	BirchMurnaghan();
	~BirchMurnaghan();
};

#endif // EOSLIB_BIRCH_MURNAGHAN_EOS_H
