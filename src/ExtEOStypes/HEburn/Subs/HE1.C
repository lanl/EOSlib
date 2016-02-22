#include "HE1.h"
#include <iostream>
#include <iomanip>
using namespace std;

const char *HE1::dname = "HE1";
const char *HE1::var_dname[] = {"lambda"};


HE1::HE1() : IDOF(HE1::dname,HE1::var_dname,1)
{
    // Null
}

HE1::HE1(const HE1 &a) : IDOF(a)
{
    // Null    
}

HE1::~HE1()
{
    // Null
}

IDOF *HE1::Copy()
{
    return new HE1(*this);
}

int HE1::InitParams(Calc &calc)
{
    z[0] = 0.0;     // unburnt, all reactants
    calc.Variable( "lambda", &z[0] );
    return 0;
}

int HE1::ConvertParams(Convert &)
{
    return 0;       // burn fraction is dimensionless
}

void HE1::PrintParams(ostream &out)
{
    out << "\tlambda = " << z[0] << "\n";
}

ostream &HE1::Print(ostream &out, const double *zz)
{
    out << "\tlambda = " << zz[0] << "\n";
    return out;
}

void HE1::Load(Calc &calc, double *zz)
{
    calc.Variable( "lambda", &zz[0] );
}
