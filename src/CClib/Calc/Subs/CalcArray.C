#include "CalcArrays.h"

CalcArray *CalcArrays::Fetch(const char *name)
{
    return arrays.Defined(name) ? &arrays.LastElement() : NULL;
}

int CalcArrays::AddF(const char *name, double *ptr, int n)
{
    if( arrays.Defined(name) )
        return 1;
    CalcArray *A = new CalcArrayF(ptr,n); 
    arrays.Replace(name, A);
    return 0;      
}

int CalcArrays::AddF(const char *name, double **ptr, int row, int col)
{
    if( arrays.Defined(name) )
        return 1;
    CalcArray *A = new CalcArrayF((double *)ptr,row, col); 
    arrays.Replace(name, A);
    return 0;      
}

int CalcArrays::AddC(const char *name, double *ptr, int n)
{
    if( arrays.Defined(name) )
        return 1;
    CalcArray *A = new CalcArray(ptr,0,n-1); 
    arrays.Replace(name, A);
    return 0;      
}

int CalcArrays::AddC(const char *name, double **ptr, int row, int col)
{
    if( arrays.Defined(name) )
        return 1;
    CalcArray *A = new CalcArray((double*)ptr,0,row-1,0,col-1); 
    arrays.Replace(name, A);
    return 0;      
}

int CalcArrays::Add(const char *name, CalcArray *&array)
{
    if( arrays.Defined(name) )
        return 1;
    arrays.Replace(name,array);
    array = NULL;
    return 0;      
}

int CalcArrays::AddDynamic(const char *name, int N, double *p, double udef)
{
    if( arrays.Defined(name) )
        return 1;
    CalcArray *A = new CalcArrayDynamic(N,p,udef); 
    arrays.Replace(name, A);
    return 0;      
}
