#include <DataBase.h>
#include "PlasticIDOF2.h"
#include <iostream>
#include <iomanip>
using namespace std;

const char *PlasticStrain2::dname = "PlasticStrain2";
const char *PlasticStrain2::var_dname[] = {"eps_pl","tauinv"};

PlasticStrain2::PlasticStrain2()
        : IDOF(PlasticStrain2::dname,PlasticStrain2::var_dname,2)
{
    z[1] = 1.0;
}

PlasticStrain2::PlasticStrain2(const PlasticStrain2 &strain) : IDOF(strain)
{
    // Null    
}

PlasticStrain2::~PlasticStrain2()
{
    // Null
}

IDOF *PlasticStrain2::Copy()
{
    return new PlasticStrain2(*this);
}

int PlasticStrain2::InitParams(Calc &calc)
{
    z[0] = 0.0;
    z[1] = 1.0;
    calc.Variable( "eps_pl", &z[0] );
    calc.Variable( "tauinv", &z[1] );
    return 0;
}

int PlasticStrain2::ConvertParams(Convert &convert)
{
    double s_t;
    if( !finite(s_t=convert.factor("time")) )
        return 1;
    z[1] /= s_t;
    return 0;
}

void PlasticStrain2::PrintParams(ostream &out)
{
    out << "\teps_pl = " << z[0] << "\n";
    out << "\ttauinv = " << z[1] << "\n";
}

ostream &PlasticStrain2::Print(ostream &out, const double *z)
{
    out << "\teps_pl = " << z[0] << "\n";
    out << "\ttauinv = " << z[1] << "\n";
    return out;
}

void PlasticStrain2::Load(Calc &calc, double *z)
{
    calc.Variable( "eps_pl", &z[0] );
    calc.Variable( "tauinv", &z[1] );
}
