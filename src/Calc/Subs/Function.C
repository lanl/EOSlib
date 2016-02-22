#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ? NULL : strdup(str);
}
#include <cmath>
#include "Calc.h"

#define FUNCTION(f) new Calc::Function(#f,f)

double calc_int(double x)
{
    if( x > 0 )
        return floor(x);
    else
        return ceil(x);
}

int Calc::init_func = 0;
Calc::Function **Calc::Functions = NULL;
void Calc::InitFunc()
{
    if( init_func )
        return;
    init_func = 1;
    static Function *Funcs[] = {
	      FUNCTION(log),  FUNCTION(exp),  FUNCTION(sqrt),
	      FUNCTION(sin),  FUNCTION(cos),  FUNCTION(tan),
	      FUNCTION(asin), FUNCTION(acos), FUNCTION(atan),
          new Calc::Function("int",calc_int),
	      NULL
	   };
    Functions = Funcs;
}

Calc::Function *Calc::ValidFunction(const char *name)
{
	Function **func;
	for( func=Functions; *func; func++ )
	{
		if( !strcmp(name,(*func)->name) )
			return *func;
	}
	return NULL;
}
	
Calc::Function::Function(const char *n, double (*f)(double))
					: name(Strdup(n)), func(f)
{
	// EMPTY
}

Calc::Function::~Function()
{
	delete [] name;
}

