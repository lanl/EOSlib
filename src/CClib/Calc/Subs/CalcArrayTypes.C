#include <cstddef>
#include "CalcArrayTypes.h"

CalcArray::CalcArray(double *p, int Mn1, int Mx1, int Mn2, int Mx2)
    : ptr(p), min1(Mn1), max1(Mx1), min2(Mn2), max2(Mx2)
{
    n1 = max1 - min1 + 1;
    n2 = max2 - min2 + 1; 
    if( ptr == NULL || n1 < 1 )
        dim = 0;    // error
    else if( n2 < 1 )
    {
        dim = 1;
        n2  = 0;
        max2 = min2 = 0;
    }
    else
        dim = 2;
}

CalcArray::~CalcArray()
{
    // Null
}

CalcArrayItr *CalcArray::Iterator(int i1)
{
    if( dim == 0 )
        return NULL;
    return new CalcArrayItr(this,i1);
}

CalcArrayItr *CalcArray::Iterator(int i1, int i2)
{
    if( dim != 2 )
        return NULL;
    return new CalcArrayItr(this,i1,i2);
}

double *CalcArray::index(int i1)
{
    if( dim == 0 || i1 < min1 || max1 < i1 )
        return NULL;
    int k = i1-min1;
    if( dim == 2 )
        k *= n1;
    return  ptr + k;
}

double *CalcArray::index(int i1, int i2)
{
    if( dim != 2 || i1 < min1 || max1 < i1 || i2 < min2 || max2 < i2 )
        return NULL;
    return  ptr + n2*(i1-min1) + (i2-min2);
}


void CalcArray::next(int &i1, int &i2)
{
    if( dim == 1 )
       ++i1;
    else
       ++i2;
}


int CalcArray::get(double &value, int i1)
{
    double *p = index(i1);
    if( p == NULL )
        return 1;
    value = *p;
    return 0;
}

int CalcArray::set(double  value, int i1)
{
    double *p = index(i1);
    if( p == NULL )
        return 1;
    *p = value;
    return 0;
}

int CalcArray::get(double &value, int i1, int i2)
{
    double *p = index(i1,i2);
    if( p == NULL )
        return 1;
    value = *p;
    return 0;
}

int CalcArray::set(double  value, int i1, int i2)
{
    double *p = index(i1,i2);
    if( p == NULL )
        return 1;
    *p = value;
    return 0;
}
//
// Iterator
//
CalcArrayItr::CalcArrayItr(CalcArray *a, int I1) : array(a), i1(I1)
{
    if( array == NULL || array->Dim() == 0 )
        return;
    if( array->Dim() == 2 )
        i2 = array->Min2();
}

CalcArrayItr::CalcArrayItr(CalcArray *a, int I1, int I2)
        : array(a), i1(I1), i2(I2)
{
    // Null
}

CalcArrayItr::~CalcArrayItr()
{
    // Null
}

int CalcArrayItr::get(double &value)
{
    double *x = ptr();
    if( x == NULL )
        return 1;
    value = *x;
    return 0;    
}

int CalcArrayItr::set(double  value)
{
    double *x = ptr();
    if( x == NULL )
        return 1;
    *x = value;
    return 0;    
}

int CalcArrayItr::next()
{
    if( array == NULL )
        return 0;
    array->next(i1,i2);
    return ptr() != NULL;   
}

double *CalcArrayItr::ptr()
{
    if( array == NULL )
        return NULL;
    return (array->Dim() == 1) ? array->index(i1) : array->index(i1,i2);
}
//
// Fortran type array
//
CalcArrayF::~CalcArrayF()
{
    // Null
}

double *CalcArrayF::index(int i1)
{
    if( dim == 0 || i1 < min1 || max1 < i1 )
        return NULL;
    int k = i1-min1;
    if( dim == 2 )
        k *= n1;
    return  ptr + k;
}

double *CalcArrayF::index(int i1, int i2)
{
    if( dim != 2 || i1 < min1 || max1 < i1 || i2 < min2 || max2 < i2 )
        return NULL;
    return  ptr + n1*(i2-min2) + (i1-min1);
}


void CalcArrayF::next(int &i1, int &i2)
{
    ++i1;
}

CalcArrayItr *CalcArrayF::Iterator(int i1)
{
    if( dim == 1 )
        return new CalcArrayItr(this,i1);
    if( dim == 2 )
        return new CalcArrayItr(this,1,i1);
    return NULL;
}
//
// Dynamic array
//
CalcArrayDynamic::CalcArrayDynamic(int N, double *p, double udef) : undef(udef)
{
    if( p != NULL )
    {
        nmax = n1 = N;
        ptr  = new double[n1];
        int i;
        for( i=0; i<n1; i++ )
           ptr[i] = p[i];
    }
    else
    {
        nmax = 0;
        n1   = (N < 2) ? 2 : N;
        ptr  = new double[n1];
        int i;
        for( i=0; i<n1; i++ )
           ptr[i] = undef;
    }
    dim = 1;
    n2  = max2 = min2 = 0;
    min1 = 1;
    max1 = n1;   
}

CalcArrayDynamic::~CalcArrayDynamic()
{
    delete [] ptr;
}

double *CalcArrayDynamic::index(int i1)
{
    int k = i1-min1;
    if( k >= 2*n1 )     // only allow doubling
        return NULL;
    if( k >= n1 && Realloc() )
        return NULL;
    if( k+1 > nmax )
        nmax = k+1;
    return  ptr + k;
}

int CalcArrayDynamic::Realloc()
{
    double *p = ptr;
    ptr = new double[2*n1];
    if( ptr == NULL )
    {
        ptr = p;
        return 1;
    }
    int i;
    for( i=0; i<n1; i++ )
        ptr[i] = p[i];
    n1 *=2;
    for( ; i<n1; i++ )
        ptr[i] = undef;
    delete [] p;
    return 0;
}

int CalcArrayDynamic::array(double *&p)
{
    if( nmax==0 )
    {
        p = NULL;
        return 0;
    }
    p = new double[nmax];
    int i;
    for( i=0; i<nmax; i++ )
       p[i] = ptr[i];
    return nmax;
}
