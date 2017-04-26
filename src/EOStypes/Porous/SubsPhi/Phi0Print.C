#include "PhiTypes.h"
#include <iostream>
#include <iomanip>
using namespace std;


void Phi0::PrintParams(ostream &out)
{
    FmtFlags old = out.setf(ios::right, ios::adjustfield);   
    out << "\t"   << setw(10) << "     V0 = " << V0
        << "; \t" << setw(10) << "     P0 = " << P0   << "\n"
        << "\t"   << setw(10) << "   phi0 = " << phi0
        << "; \t" << setw(10) << "     Pc = " << Pc   << "\n"
        << "\t"   << setw(10) << "   PV_0 = " << PV_0
        << "; \t" << setw(10) << "   PV_1 = " << PV_1 << "\n";
    if( scale )
        out << "\tscale = 1\n";
    out.setf(old, ios::adjustfield);    
    PhiEq_spline::PrintParams(out);
}
