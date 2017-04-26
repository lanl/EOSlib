#ifndef EOSLIB_BIRCH_MURNAGHAN_EOS_VT_H
#define EOSLIB_BIRCH_MURNAGHAN_EOS_VT_H

#include "GenHayes_VT.h"
#include "BirchMurnaghanParams.h"

#define BirchMurnaghan_VT_vers "BirchMurnaghan_VTv2.0.1"
#define BirchMurnaghan_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *BirchMurnaghan_VTlib_vers;
extern const char *BirchMurnaghan_VTlib_date;

extern "C" { // DataBase functions
    char *EOS_VT_BirchMurnaghan_Init();
    char *EOS_VT_BirchMurnaghan_Info();
    void *EOS_VT_BirchMurnaghan_Constructor();
}
//
//
class EOS;
class BirchMurnaghan;
//
//
class BirchMurnaghan_VT : public GenHayes_VT, public BirchMurnaghanBase
{
    friend class BirchMurnaghan;
private:
	void operator=(const BirchMurnaghan_VT&);	  // disallowed
	BirchMurnaghan_VT(const BirchMurnaghan_VT&);  // disallowed
	BirchMurnaghan_VT *operator &();			  // disallowed, use Duplicate()
protected:
	BirchMurnaghan_VT(BirchMurnaghan &eos);
    EOS *Ve();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
	BirchMurnaghan_VT();
	~BirchMurnaghan_VT();
};

#endif // EOSLIB_BIRCH_MURNAGHAN_EOS_VT_H
