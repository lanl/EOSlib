#ifndef CCLIB_CALC_VARS_H
#define CCLIB_CALC_VARS_H

#include <NameArray.h>
#include "CalcVarTypes.h"

class CalcVars 
{
private:
	CalcVars(const CalcVars&);			    // disallowed
	void operator=(const CalcVars&);		// disallowed

    NAMEARRAY<CalcVar> variables;
public:
	CalcVars(int d = 16) : variables(d) {}
	~CalcVars() {}

    CalcVar::TYPE type(const char *name);
    CalcVar *Fetch(const char *var);
    CalcVar *Last() { return &variables.LastElement(); }

    int AddVar(const char *name, CalcVar *&var);
    int AddVar(const char *name, double *var);
    int AddVar(const char *name, double var);
    int AddVar(const char *name, int *var);
    int AddVar(const char *name, int var);
    int AddVar(const char *name, char **var);
    int AddVar(const char *name, const char *var);
   
    int get(const char *var, double &value, double *undef=NULL);
	int set(const char *var, double value,  int undef=0);
            
	int get(const char *var, const char *&value, const char *undef=NULL);
	int set(const char *var, const char *value, int undef=0);

// add itterator
};


#endif // CCLIB_CALC_VARS_H
