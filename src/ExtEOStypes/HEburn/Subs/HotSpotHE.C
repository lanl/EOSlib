#include <iostream>
#include <iomanip>
using namespace std;

#include "HotSpotHE.h"
#include <DataBase.h>

const char *HotSpotHE::dname = "HotSpotHE";
const char *HotSpotHE::var_dname[] = {"Ps","ts","lambda","s"};


HotSpotHE::HotSpotHE() : IDOF(HotSpotHE::dname,HotSpotHE::var_dname,4)
{
    // Null
}

HotSpotHE::HotSpotHE(const HotSpotHE &a) : IDOF(a)
{
    // Null    
}

HotSpotHE::~HotSpotHE()
{
    // Null
}

IDOF *HotSpotHE::Copy()
{
    return new HotSpotHE(*this);
}

int HotSpotHE::InitParams(Calc &calc)
{
    z[0] = 0.0;     // Ps, shock pressure
    z[1] = 0.0;     // ts, shock arrival time
    z[2] = 0.0;     // lambda, all reactants (unburnt)
    z[3] = 0.0;     // s, scaled reaction distance
    calc.Variable( "Ps",     &z[0] );
    calc.Variable( "ts",     &z[1] );
    calc.Variable( "lambda", &z[2] );
    calc.Variable( "s",      &z[3] );
    return 0;
}

int HotSpotHE::ConvertParams(Convert &convert)
{
    double s_P, s_t;
    if( !finite(s_P=convert.factor("P")) || !finite(s_t=convert.factor("time")) )
        return 1;
    z[0] *= s_P;
    z[1] *= s_t;
    // z[2] = lambda is dimensionless
    // z[3] = scaled is dimensionless
    return 0;
}

void HotSpotHE::PrintParams(ostream &out)
{
    out << "\tPs     = " << z[0] << "\n";
    out << "\tts     = " << z[1] << "\n";
    out << "\tlambda = " << z[2] << "\n";
    out << "\ts      = " << z[3] << "\n";
}

ostream &HotSpotHE::Print(ostream &out, const double *zz)
{
    out << "\tPs     = " << zz[0] << "\n";
    out << "\tts     = " << zz[1] << "\n";
    out << "\tlambda = " << zz[2] << "\n";
    out << "\ts      = " << zz[3] << "\n";
    return out;
}

void HotSpotHE::Load(Calc &calc, double *zz)
{
    calc.Variable( "Ps",     &zz[0] );
    calc.Variable( "ts",     &zz[1] );
    calc.Variable( "lambda", &zz[2] );
    calc.Variable( "s",      &zz[3] );
}
