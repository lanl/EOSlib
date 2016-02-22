#ifndef EOSLIB_KEANE_EOS_VT_H
#define EOSLIB_KEANE_EOS_VT_H

#include "GenHayes_VT.h"
#include "KeaneParams.h"

#define Keane_VT_vers "Keane_VTv2.0.1"
#define Keane_VT_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *Keane_VTlib_vers;
extern const char *Keane_VTlib_date;

extern "C" { // DataBase functions
    char *EOS_VT_Keane_Init();
    char *EOS_VT_Keane_Info();
    void *EOS_VT_Keane_Constructor();
}
//
//
class EOS;
class Keane;
//
//
class Keane_VT : public GenHayes_VT, public KeaneBase
{
    friend class Keane;
private:
	void operator=(const Keane_VT&);    // disallowed
	Keane_VT(const Keane_VT&);          // disallowed
	Keane_VT *operator &();			    // disallowed, use Duplicate()
protected:
	Keane_VT(Keane &eos);
    EOS *Ve();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
	Keane_VT();
	~Keane_VT();
};

#endif // EOSLIB_KEANE_EOS_VT_H
