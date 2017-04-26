#include "PhiTypes.h"
#include <iostream>
#include <iomanip>
using namespace std;

void Phi1::PrintParams(ostream &out)
{
    FmtFlags old = out.setf(ios::right, ios::adjustfield);
    out << "\t"   << setw(10) << "     V0 = " <<    V0
        << "; \t" << setw(10) << "     P0 = " <<    P0   << "\n"
        << "\t"   << setw(10) << "    PV0 = " <<    PV0
        << "; \t" << setw(10) << "    PV1 = " <<    PV1  << "\n"
        << "\t"   << setw(10) << "   phi0 = " <<    phi0
        << "; \t" << setw(10) << "   phi1 = " <<    phi1 << "\n"
        << "\t"   << setw(10) << "      n = " <<       n
        << "; \t" << setw(10) << "      a = " <<       a << "\n"
        << "\t"   << setw(10) << "dphidP0 = " << dphidP0
        << "; \t" << setw(10) << "dphidP1 = " << dphidP1 << "\n"
        << "\t"   << setw(10) << "  sphi0 = " <<   sphi0
        << "; \t" << setw(10) << "   sPV1 = " <<    sPV1/PV1 << "\n";
    out.setf(old, ios::adjustfield);
    PhiEq_spline::PrintParams(out);
}
