#include "AvArrheniusIDOF.h"
#include <DataBase.h>
#include <iostream>
#include <iomanip>
using namespace std;

const char *AvArrheniusIDOF::dname = "AvArrheniusIDOF";
const char *AvArrheniusIDOF::var_dname[] = {"lambda","Tav"};


AvArrheniusIDOF::AvArrheniusIDOF()
        : IDOF(AvArrheniusIDOF::dname,AvArrheniusIDOF::var_dname,2)
{
    // Null
}

AvArrheniusIDOF::AvArrheniusIDOF(const AvArrheniusIDOF &a) : IDOF(a)
{
    // Null    
}

AvArrheniusIDOF::~AvArrheniusIDOF()
{
    // Null
}

IDOF *AvArrheniusIDOF::Copy()
{
    return new AvArrheniusIDOF(*this);
}

int AvArrheniusIDOF::InitParams(Calc &calc)
{
    z[0] = 0.0;     // lambda, all reactants
    z[1] = 0.0;     // Tav
    calc.Variable( "lambda", &z[0] );
    calc.Variable( "Tav",    &z[1] );
    return 0;
}

int AvArrheniusIDOF::ConvertParams(Convert &convert)
{
    double s_T;
    if( !finite(s_T=convert.factor("T")) )
        return 1;
    z[1] *= s_T;
    return 0;
}

void AvArrheniusIDOF::PrintParams(ostream &out)
{
    out << "\tlambda = " << z[0] << "\n";
    out << "\tTav    = " << z[1] << "\n";
}

ostream &AvArrheniusIDOF::Print(ostream &out, const double *zz)
{
    out << "\tlambda = " << zz[0] << "\n";
    out << "\tTav    = " << zz[1] << "\n";
    return out;
}

void AvArrheniusIDOF::Load(Calc &calc, double *zz)
{
    calc.Variable( "lambda", &zz[0] );
    calc.Variable( "Tav",    &zz[1] );
}
