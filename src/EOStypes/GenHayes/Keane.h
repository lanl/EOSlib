#ifndef EOSLIB_KEANE_EOS_H
#define EOSLIB_KEANE_EOS_H

#include "GenHayes.h"
#include "KeaneParams.h"

#define Keane_vers "Keanev2.0.1"
#define Keane_date "Jan. 5, 2013"
#undef  BugReport
#define BugReport "Please report bugs to rtm@lanl.gov"

extern const char *Keanelib_vers;
extern const char *Keanelib_date;

extern "C" { // DataBase functions
    char *EOS_Keane_Init();
    char *EOS_Keane_Info();
    void *EOS_Keane_Constructor();
}
//
//
class EOS_VT;
class Keane_VT;
//
//  GenHayes form of EOS with
//    Keane cold curve
//    Specific heat is function of single scaled Debye temperature
//      Cv(V,T) = SCv(V/theta(V))
//    Gruneisen coefficient is function of only V
//      Gamma(V) = a + b*V/V0 = - dln[theta(V)]/dln(V)
//
class Keane : public GenHayes, public KeaneBase
{
    friend class Keane_VT;
private:
	void operator=(const Keane&);	// disallowed
	Keane(const Keane&);            // disallowed
	Keane *operator &();			// disallowed, use Duplicate()
protected:
	Keane(Keane_VT &eos);
    EOS_VT *VT();
    int InitParams(Parameters &, Calc &, DataBase *db=NULL);
    void PrintParams(ostream &out);
    int ConvertParams(Convert &convert);    
public:
	Keane();
	~Keane();
};

#endif // EOSLIB_KEANE_EOS_H
