#include "PhiTypes.h"
#include <iostream>
using std::ostream;

void EC::PrintParams(ostream &out)
{
    out << "\texpt = \""   << (expt ? expt : "NULL") << "\"\n"
        << "\t hmx = \""   << ( hmx ?  hmx : "NULL") << "\"\n";
    PhiEq_spline::PrintParams(out);
}
