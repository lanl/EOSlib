#include "HotSpotCC_HE.h"
#include <DataBase.h>
#include <iostream>
#include <iomanip>
using namespace std;

const char *HotSpotCC_HE::dname = "HotSpotCC_HE";
const char *HotSpotCC_HE::var_dname[] = {"Ps","ts","lambda","s","s2","e_cc"};


HotSpotCC_HE::HotSpotCC_HE()
              : IDOF(HotSpotCC_HE::dname,HotSpotCC_HE::var_dname,6)
{
    // Null
}

HotSpotCC_HE::HotSpotCC_HE(const HotSpotCC_HE &a) : IDOF(a)
{
    // Null    
}

HotSpotCC_HE::~HotSpotCC_HE()
{
    // Null
}

IDOF *HotSpotCC_HE::Copy()
{
    return new HotSpotCC_HE(*this);
}

int HotSpotCC_HE::InitParams(Calc &calc)
{
    z[0] = 0.0;     // Ps,      shock pressure
    z[1] = 0.0;     // ts,      shock arrival time
    z[2] = 0.0;     // lambda,  all reactants (unburnt)
    z[3] = 0.0;     // s,       scaled reaction distance
    z[4] = 0.0;     // s2,      no carbon clusters
    z[5] = 0.0;     // e_cc,    carbon cluster specific energy
    calc.Variable( "Ps",      &z[0] );
    calc.Variable( "ts",      &z[1] );
    calc.Variable( "lambda",  &z[2] );
    calc.Variable( "s",       &z[3] );
    calc.Variable( "s2",      &z[4] );
    calc.Variable( "e_cc",    &z[5] );
    return 0;
}

int HotSpotCC_HE::ConvertParams(Convert &convert)
{
    double s_P, s_t, s_e;
    if( !finite(s_P=convert.factor("P"))
        || !finite(s_t=convert.factor("time"))
        || !finite(s_e=convert.factor("e")) )
            return 1;
    z[0] *= s_P;
    z[1] *= s_t;
    // z[2-4] are dimensionless
    z[5] *= s_e;
    return 0;
}

void HotSpotCC_HE::PrintParams(ostream &out)
{
    out << "\tPs      = " << z[0] << "\n";
    out << "\tts      = " << z[1] << "\n";
    out << "\tlambda  = " << z[2] << "\n";
    out << "\ts       = " << z[3] << "\n";
    out << "\ts2      = " << z[4] << "\n";
    out << "\te_cc    = " << z[5] << "\n";
}

ostream &HotSpotCC_HE::Print(ostream &out, const double *zz)
{
    out << "\tPs      = " << zz[0] << "\n";
    out << "\tts      = " << zz[1] << "\n";
    out << "\tlambda  = " << zz[2] << "\n";
    out << "\ts       = " << zz[3] << "\n";
    out << "\ts2      = " << zz[4] << "\n";
    out << "\te_cc    = " << zz[5] << "\n";
    return out;
}

void HotSpotCC_HE::Load(Calc &calc, double *zz)
{
    calc.Variable( "Ps",      &zz[0] );
    calc.Variable( "ts",      &zz[1] );
    calc.Variable( "lambda",  &zz[2] );
    calc.Variable( "s",       &zz[3] );
    calc.Variable( "s2",      &zz[4] );
    calc.Variable( "e_cc",    &zz[5] );
}
