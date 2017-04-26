#ifndef EOSLIB_ARRHENIUS_IDOF_H
#define EOSLIB_ARRHENIUS_IDOF_H
#include <IDOF.h>
#include <Calc.h>
//
//  Arrhenius HE kinetics
//  lambda = reaction progress variable (mass fraction of products)
//          0 = reactants (unburnt)
//          1 = products (burnt)
//  Rate = (1-lambda)*k*exp(-T_a/T)
//         T_a = activation temperature
//         k = rate multiplier (frequency factor)
//
class ArrheniusIDOF : public IDOF
{ // n = 1
  // z[0] = lambda
public:
    static const char *dname;
    static const char *var_dname[];              // name of z[i]    
    // int IDOF::n;     // number of variables
    // double *IDOF::z; // variables
    ArrheniusIDOF();
    ArrheniusIDOF(const ArrheniusIDOF &);
    ~ArrheniusIDOF();
    IDOF *Copy();
 //
    int InitParams(Calc &);
    int ConvertParams(Convert &);
    void PrintParams(std::ostream &);
 //
    std::ostream &Print(std::ostream &, const double *);
    void Load(Calc &, double *);
};

#endif // EOSLIB_ARRHENIUS_IDOF_H
