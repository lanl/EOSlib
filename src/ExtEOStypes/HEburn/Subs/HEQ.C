#include "HEQ.h"
#include <DataBase.h>
#include <iostream>
#include <iomanip>
using namespace std;


const char *HEQ::dname       = "HEQ";
const char *HEQ::var_dname[] = {"lambda","Q"};


HEQ::HEQ() : IDOF(HEQ::dname,HEQ::var_dname,2)
{
    // Null
}

HEQ::HEQ(const HEQ &a) : IDOF(a)
{
    // Null    
}

HEQ::~HEQ()
{
    // Null
}

IDOF *HEQ::Copy()
{
    return new HEQ(*this);
}

int HEQ::InitParams(Calc &calc)
{
    z[0] = 0.0;     // unburnt, all reactants
    z[1] = 0.0;     // Q, viscous pressure
    calc.Variable( "lambda", &z[0] );
    calc.Variable( "Q",      &z[1] );
    return 0;
}

int HEQ::ConvertParams(Convert &convert)
{
    double s_P;
    if( !finite(s_P=convert.factor("P")) )
        return 1;
    // z[0] = lambda is dimensionless
    z[1] *= s_P;
    return 0;
}

void HEQ::PrintParams(ostream &out)
{
    out << "\tlambda = " << z[0] << "\n"
        << "\tQ      = " << z[1] << "\n";
}

ostream &HEQ::Print(ostream &out, const double *zz)
{
    out << "\tlambda = " << zz[0] << "\n"
        << "\tQ      = " << zz[1] << "\n";
    return out;
}

void HEQ::Load(Calc &calc, double *zz)
{
    calc.Variable( "lambda", &zz[0] );
    calc.Variable( "Q",      &zz[1] );
}
