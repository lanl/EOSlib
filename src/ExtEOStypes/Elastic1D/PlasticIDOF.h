#ifndef EOSLIB_PLASTIC_IDOF_H
#define EOSLIB_PLASTIC_IDOF_H

#include <IDOF.h>
#include <Calc.h>

#include <iosfwd>

//
// Total deformation for uniaxial strain (1D)
//    F = diag[V/V0, 1, 1]
// Plastic strain tensor is assumed to have form
//         Cp = diag[x_pl^2, 1/x_pl, 1/x_pl]
// where x_pl = exp(2*eps_pl/3) and eps_pl is scalar plastic strain variable.
// Elastic tensor can be expressed as
//   b_el(V,eps_el) = (V/V0)^{2/3}*diag[x_el^2, 1/x_el, 1/x_el]
// and x_el = exp(2*eps_el/3), with eps_el scalar elastic strain variable
//   eps_el(V,eps_pl) = ln(V/V0) - eps_pl
// Note, with eps_pl = 0, elastic strain variable is
//   eps_el ~= V/V0 - 1, negative in compression !
//
class PlasticStrain1 : public IDOF
{ // n = 1 and z[0] = eps_pl
public:
    static const char *dname;
    static const char *var_dname[];              // name of z[i]    
    // int IDOF::n;     // number of variables
    // double *IDOF::z; // variables
    PlasticStrain1();
    PlasticStrain1(const PlasticStrain1 &);
    ~PlasticStrain1();
    IDOF *Copy();
 //
    int InitParams(Calc &);
    int ConvertParams(Convert &);
    void PrintParams(std::ostream &);
 //
    std::ostream &Print(std::ostream &, const double *);
    void Load(Calc &, double *);
};

#endif // EOSLIB_PLASTIC_IDOF_H
