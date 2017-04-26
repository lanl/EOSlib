#include "ElasticPlastic.h"

const char *PlasticStrain::dname = "PlasticStrain";
const char *PlasticStrain::var_dname[] = {"eps_pl"};

PlasticStrain::PlasticStrain()
        : IDOF(PlasticStrain::dname,PlasticStrain::var_dname,1)
{
    // Null
}

PlasticStrain::PlasticStrain(const PlasticStrain &strain) : IDOF(strain)
{
    // Null    
}

PlasticStrain::~PlasticStrain()
{
    // Null
}

IDOF *PlasticStrain::Copy()
{
    return new PlasticStrain(*this);
}

int PlasticStrain::InitParams(Calc &calc)
{
    calc.Variable( "eps_pl", &z[0] );
    return 0;
}

int PlasticStrain::ConvertParams(Convert &)
{
    return 0;   // strain is dimensionless
}

void PlasticStrain::PrintParams(ostream &out)
{
    out << "\teps_pl = " << z[0] << "\n";
}

ostream &PlasticStrain::Print(ostream &out, const double *z)
{
    out << "\teps_pl = " << z[0] << "\n";
    return out;
}

void PlasticStrain::Load(Calc &calc, double *z)
{
    calc.Variable( "eps_pl", &z[0] );
}
