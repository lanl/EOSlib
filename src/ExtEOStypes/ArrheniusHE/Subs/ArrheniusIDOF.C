#include "ArrheniusIDOF.h"
#include <iostream>
#include <iomanip>
using namespace std;

const char *ArrheniusIDOF::dname = "ArrheniusIDOF";
const char *ArrheniusIDOF::var_dname[] = {"lambda"};


ArrheniusIDOF::ArrheniusIDOF()
        : IDOF(ArrheniusIDOF::dname,ArrheniusIDOF::var_dname,1)
{
    // Null
}

ArrheniusIDOF::ArrheniusIDOF(const ArrheniusIDOF &a) : IDOF(a)
{
    // Null    
}

ArrheniusIDOF::~ArrheniusIDOF()
{
    // Null
}

IDOF *ArrheniusIDOF::Copy()
{
    return new ArrheniusIDOF(*this);
}

int ArrheniusIDOF::InitParams(Calc &calc)
{
    z[0] = 0.0;     // unburnt, all reactants
    calc.Variable( "lambda", &z[0] );
    return 0;
}

int ArrheniusIDOF::ConvertParams(Convert &)
{
    return 0;       // burn fraction is dimensionless
}

void ArrheniusIDOF::PrintParams(ostream &out)
{
    out << "\tlambda = " << z[0] << "\n";
}

ostream &ArrheniusIDOF::Print(ostream &out, const double *zz)
{
    out << "\tlambda = " << zz[0] << "\n";
    return out;
}

void ArrheniusIDOF::Load(Calc &calc, double *zz)
{
    calc.Variable( "lambda", &zz[0] );
}
