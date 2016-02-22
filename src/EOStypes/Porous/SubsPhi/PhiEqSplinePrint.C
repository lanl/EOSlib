#include "PhiEqSpline.h"
#include <iostream>
using std::ostream;

void PhiEq_spline::PrintParams(ostream &out)
{
    out << "\tabs_tol = "       << abs_tol
            << "; \trel_tol = " << rel_tol         << "\n"
        << "\t# knots: phi = "  << Nknot()
            << " \tenergy = "   << Espline.Nknot() << "\n";
    PhiEq::PrintParams(out);
}
