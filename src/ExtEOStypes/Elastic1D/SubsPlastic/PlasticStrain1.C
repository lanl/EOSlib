#include "PlasticIDOF.h"
#include <iostream>
#include <iomanip>
using namespace std;

const char *PlasticStrain1::dname = "PlasticStrain1";
const char *PlasticStrain1::var_dname[] = {"eps_pl"};


PlasticStrain1::PlasticStrain1()
        : IDOF(PlasticStrain1::dname,PlasticStrain1::var_dname,1)
{
    // Null
}

PlasticStrain1::PlasticStrain1(const PlasticStrain1 &strain) : IDOF(strain)
{
    // Null    
}

PlasticStrain1::~PlasticStrain1()
{
    // Null
}

IDOF *PlasticStrain1::Copy()
{
    return new PlasticStrain1(*this);
}

int PlasticStrain1::InitParams(Calc &calc)
{
    z[0] = 0.0;
    calc.Variable( "eps_pl", &z[0] );
    return 0;
}

int PlasticStrain1::ConvertParams(Convert &)
{
    return 0;   // strain is dimensionless
}

void PlasticStrain1::PrintParams(ostream &out)
{
    out << "\teps_pl = " << z[0] << "\n";
}

ostream &PlasticStrain1::Print(ostream &out, const double *z)
{
    out << "\teps_pl = " << z[0] << "\n";
    return out;
}

void PlasticStrain1::Load(Calc &calc, double *z)
{
    calc.Variable( "eps_pl", &z[0] );
}
