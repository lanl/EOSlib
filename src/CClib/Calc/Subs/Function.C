#include <cstring>
inline char* Strdup(const char* str)
{
    return (str==NULL) ? NULL : strdup(str);
}
#include <cmath>
#include "Calc.h"

//#include <iostream>     // debugging

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
//std::cout << "DEBUG: Calc::InitFunc, this " << this;
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
//std::cout << " init_func " << init_func
//          << "  Functions " << Functions
//          << " ( " << &Functions << ")\n";
}

Calc::Function *Calc::ValidFunction(const char *name)
{
	Function **func;
    /***/
    if( Functions == NULL )
    {
//std::cout << "DEBUG: Calc::ValidFunction, this " << this
//          << "  init_func " << init_func << "\n";
        // On Mac, with shared objects,
        // static variables init_func and Functions get reset
        // But not on linux with gcc
        InitFunc();
    }
    /***/
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

