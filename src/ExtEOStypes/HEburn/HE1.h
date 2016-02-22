#ifndef EOSLIB_HE1_H
#define EOSLIB_HE1_H
#include <IDOF.h>
#include <Calc.h>
//
//  IDOF for reaction model with single reaction progress variable
//  lambda = reaction progress variable (mass fraction of products)
//          0 = reactants (unburnt)
//          1 = products (burnt)
//
class HE1 : public IDOF
{
public:
    static const char *dname;           // "HE1"
    static const char *var_dname[];     // name of z[i] = {"lambda"}
    // int IDOF::n;                     // number of variables = 1
    // double *IDOF::z;                 // variables, z = {lambda}
    HE1();
    HE1(const HE1 &);
    ~HE1();
    IDOF *Copy();
 //
    int InitParams(Calc &);
    int ConvertParams(Convert &);
    void PrintParams(std::ostream &);
 //
    std::ostream &Print(std::ostream &, const double *);
    void Load(Calc &, double *);
};

#endif // EOSLIB_HE1_H
