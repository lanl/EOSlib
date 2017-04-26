#include <iostream>
#include <iomanip>
using namespace std;

#include "HotSpotHE2.h"
#include <DataBase.h>

const char *HotSpotHE2::dname = "HotSpotHE2";
const char *HotSpotHE2::var_dname[] = {"Ps","Ts","ts","lambda","s","s0"};


HotSpotHE2::HotSpotHE2() : IDOF(HotSpotHE2::dname,HotSpotHE2::var_dname,6)
{
    // Null
}

HotSpotHE2::HotSpotHE2(const HotSpotHE2 &a) : IDOF(a)
{
    // Null    
}

HotSpotHE2::~HotSpotHE2()
{
    // Null
}

IDOF *HotSpotHE2::Copy()
{
    return new HotSpotHE2(*this);
}

int HotSpotHE2::InitParams(Calc &calc)
{
    z[0] = 0.0;     // Ps, shock pressure
    z[1] = 0.0;     // Ts, shock temperature
    z[2] = 0.0;     // ts, shock arrival time
    z[3] = 0.0;     // lambda, all reactants (unburnt)
    z[4] = 0.0;     // s,  scaled reaction distance
    z[5] = 0.0;     // s0, (d/dt)s at shock front
    calc.Variable( "Ps",     &z[0] );
    calc.Variable( "Ts",     &z[1] );
    calc.Variable( "ts",     &z[2] );
    calc.Variable( "lambda", &z[3] );
    calc.Variable( "s",      &z[4] );
    calc.Variable( "s0",     &z[5] );
    return 0;
}

int HotSpotHE2::ConvertParams(Convert &convert)
{
    double s_P, s_T, s_t;
    if( !finite(s_P=convert.factor("P"))
      || !finite(s_T=convert.factor("T"))
      || !finite(s_t=convert.factor("time")) )
        return 1;
    z[0] *= s_P;
    z[1] *= s_T;
    z[2] *= s_t;
    // z[3] = lambda is dimensionless
    // z[4] = s  is dimensionless
    z[5] /= s_t;
    return 0;
}

void HotSpotHE2::PrintParams(ostream &out)
{
    out << "\tPs     = " << z[0] << "\n";
    out << "\tTs     = " << z[1] << "\n";
    out << "\tts     = " << z[2] << "\n";
    out << "\tlambda = " << z[3] << "\n";
    out << "\ts      = " << z[4] << "\n";
    out << "\ts0     = " << z[5] << "\n";
}

ostream &HotSpotHE2::Print(ostream &out, const double *zz)
{
    out << "\tPs     = " << zz[0] << "\n";
    out << "\tTs     = " << zz[1] << "\n";
    out << "\tts     = " << zz[2] << "\n";
    out << "\tlambda = " << zz[3] << "\n";
    out << "\ts      = " << zz[4] << "\n";
    out << "\ts0     = " << zz[5] << "\n";
    return out;
}

void HotSpotHE2::Load(Calc &calc, double *zz)
{
    calc.Variable( "Ps",     &zz[0] );
    calc.Variable( "Ts",     &zz[1] );
    calc.Variable( "ts",     &zz[2] );
    calc.Variable( "lambda", &zz[3] );
    calc.Variable( "s",      &zz[4] );
    calc.Variable( "s0",     &zz[5] );
}
