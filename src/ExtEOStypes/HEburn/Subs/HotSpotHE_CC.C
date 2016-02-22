#include <iostream>
#include <iomanip>
using namespace std;

#include "HotSpotHE_CC.h"
#include <DataBase.h>

const char *HotSpotHE_CC::dname = "HotSpotHE_CC";
const char *HotSpotHE_CC::var_dname[] = {"Ps","ts","s","s2"};


HotSpotHE_CC::HotSpotHE_CC()
              : IDOF(HotSpotHE_CC::dname,HotSpotHE_CC::var_dname,4)
{
    // Null
}

HotSpotHE_CC::HotSpotHE_CC(const HotSpotHE_CC &a) : IDOF(a)
{
    // Null    
}

HotSpotHE_CC::~HotSpotHE_CC()
{
    // Null
}

IDOF *HotSpotHE_CC::Copy()
{
    return new HotSpotHE_CC(*this);
}

int HotSpotHE_CC::InitParams(Calc &calc)
{
    z[0] = 0.0;     // Ps,      shock pressure
    z[1] = 0.0;     // ts,      shock arrival time
    z[2] = 0.0;     // s,       lambda  = g(s)
    z[3] = 0.0;     // s2,      lambda2 = s2**2
    calc.Variable( "Ps",      &z[0] );
    calc.Variable( "ts",      &z[1] );
    calc.Variable( "s",       &z[2] );
    calc.Variable( "s2",      &z[3] );
    return 0;
}

int HotSpotHE_CC::ConvertParams(Convert &convert)
{
    double s_P, s_t;
    if(    !finite(s_P=convert.factor("P"))
        || !finite(s_t=convert.factor("time")) )
            return 1;
    z[0] *= s_P;
    z[1] *= s_t;
    // z[2-3] are dimensionless
    return 0;
}

void HotSpotHE_CC::PrintParams(ostream &out)
{
    out << "\tPs      = " << z[0] << "\n";
    out << "\tts      = " << z[1] << "\n";
    out << "\ts       = " << z[2] << "\n";
    out << "\ts2      = " << z[3] << "\n";
}

ostream &HotSpotHE_CC::Print(ostream &out, const double *zz)
{
    out << "\tPs      = " << zz[0] << "\n";
    out << "\tts      = " << zz[1] << "\n";
    out << "\ts       = " << zz[2] << "\n";
    out << "\ts2      = " << zz[3] << "\n";
    return out;
}

void HotSpotHE_CC::Load(Calc &calc, double *zz)
{
    calc.Variable( "Ps",      &zz[0] );
    calc.Variable( "ts",      &zz[1] );
    calc.Variable( "s",       &zz[2] );
    calc.Variable( "s2",      &zz[3] );
}
