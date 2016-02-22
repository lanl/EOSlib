#include "IDOF.h"
#include <Calc.h>
#include <iostream>
using namespace std;

IDOF::IDOF(const char *dname, const char **var_dname, int nn)
    : name(dname), var_name(var_dname), n(0), z(NULL)
{
    if( nn<=0 )
        return;
    n = nn;
    z = new double[n];
    int i;
    for( i=0; i<n; ++i )
        z[i] = 0.0;    
}

IDOF::IDOF(const IDOF &d) : name(d.name), var_name(d.var_name)
{
    n = d.n;
    z = new double[n];
    int i;
    for( i=0; i<n; ++i )
        z[i] = d.z[i];
}

int IDOF::var_index(const char *vname) const
{
    if( var_name == NULL )
        return -1;
    int i;
    for( i=0; i<n; i++ )
    {
       if( !strcmp(vname, var_name[i]) )
           return i; 
    }
    return -1;
}

IDOF::~IDOF()
{
    delete [] z;
}

int IDOF::Replace(const IDOF &d)
{
    if( n != d.n )
        return 1;
    int i;
    for( i=0; i<n; ++i )
        z[i] = d.z[i];
    return 0;
}
//
// virtual function to be supplied by derived class
// 
IDOF *IDOF::Copy()
{
    return NULL;
}

int IDOF::InitParams(Calc &)
{
    return 1;
}

int IDOF::ConvertParams(Convert &)
{
    return 1;
}

void IDOF::PrintParams(ostream &)
{
   // Null 
}

ostream &IDOF::Print(ostream &out, const double *)
{
   return out; 
}

void IDOF::Load(Calc &, double *)
{
    // Null
}

void IDOF::SetParams(Calc &calc)
{
    if( var_name == NULL )
        return;
    int i;
    for( i=0; i<n; i++ )
        calc.Fetch(var_name[i],z[i]);
}
