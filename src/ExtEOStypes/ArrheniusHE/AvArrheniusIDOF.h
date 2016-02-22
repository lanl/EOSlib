#ifndef EOSLIB_AV_ARRHENIUS_IDOF_H
#define EOSLIB_AV_ARRHENIUS_IDOF_H
#include <IDOF.h>
#include <Calc.h>
//
//  Arrhenius kinetics with averaged temperature
//  lambda = reaction progress variable (mass fraction of products)
//          0 = reactants (unburnt)
//          1 = products  (burnt)
//  Tav    = time averaged temperature
//
//  (d/dt)lambda = (1-lambda)*k*exp(-T_a/Tav)
//           T_a = activation temperature
//             k = rate multiplier (frequency factor)
//  (d/dt)Tav    = (T-Tav)/tau
//           T   = material temperature
//           tau = time constant
//
class AvArrheniusIDOF : public IDOF
{ // n = 2
  // z[0] = lambda
  // z[1] = Tav
public:
    static const char *dname;
    static const char *var_dname[];              // name of z[i]    
    // int IDOF::n;     // number of variables
    // double *IDOF::z; // variables
    AvArrheniusIDOF();
    AvArrheniusIDOF(const AvArrheniusIDOF &);
    ~AvArrheniusIDOF();
    IDOF *Copy();
 //
    int InitParams(Calc &);
    int ConvertParams(Convert &);
    void PrintParams(std::ostream &);
 //
    std::ostream &Print(std::ostream &, const double *);
    void Load(Calc &, double *);
};

#endif // EOSLIB_AV_ARRHENIUS_IDOF_H
