#include "Elastic.h"

const char *ElasticStrain::dname = "ElasticStrain";
const char *ElasticStrain::var_dname[] = {"eps_el"};

ElasticStrain::ElasticStrain()
        : IDOF(ElasticStrain::dname,ElasticStrain::var_dname,1)
{
    // Null
}

ElasticStrain::ElasticStrain(const ElasticStrain &strain) : IDOF(strain)
{
    // Null    
}

ElasticStrain::~ElasticStrain()
{
    // Null
}

IDOF *ElasticStrain::Copy()
{
    return new ElasticStrain(*this);
}

int ElasticStrain::InitParams(Calc &calc)
{
    calc.Variable( "eps_el", &z[0] );
    return 0;
}

int ElasticStrain::ConvertParams(Convert &)
{
    return 0;   // strain is dimensionless
}

void ElasticStrain::PrintParams(ostream &out)
{
    out << "\teps_el = " << z[0] << "\n";
}

ostream &ElasticStrain::Print(ostream &out, const double *z)
{
    out << "\teps_el = " << z[0] << "\n";
    return out;
}

void ElasticStrain::Load(Calc &calc, double *z)
{
    calc.Variable( "eps_el", &z[0] );
}
