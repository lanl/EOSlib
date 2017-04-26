#ifndef CCLIB_CALC_ARRAYS_H
#define CCLIB_CALC_ARRAYS_H

#include <NameArray.h>
#include "CalcArrayTypes.h"

class CalcArrays
{
private:
	CalcArrays(const CalcArrays&);			// disallowed
	void operator=(const CalcArrays&);		// disallowed

    NAMEARRAY<CalcArray> arrays;
public:
	CalcArrays(int d = 4) : arrays(d) {}
	~CalcArrays() {}
    
    CalcArray *Fetch(const char *name);
    CalcArray *Last() { return &arrays.LastElement(); }

	int AddF(const char *name, double *ptr, int n);
	int AddF(const char *name, double **ptr, int row, int col);
	int AddC(const char *name, double *ptr, int n);
	int AddC(const char *name, double **ptr, int row, int col);
	int Add(const char *name, CalcArray *&array);
	int AddDynamic(const char *name, int N=32, double *p=NULL, double udef=0.0);
};

#endif // CCLIB_CALC_ARRAYS_H
