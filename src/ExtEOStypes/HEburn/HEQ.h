#ifndef EOSLIB_HEQ_H
#define EOSLIB_HEQ_H
#include <IDOF.h>
#include <Calc.h>
//
//  IDOF for Forest Fire rate model, z = {lambda,Q}
//  Forest Fire rate = rate(lambda,P+Q)
//      where Q is viscous pressure and is to be set by solver
//  lambda = reaction progress variable (mass fraction of products)
//          0 = reactants (unburnt)
//          1 = products (burnt)
//
class HEQ : public IDOF
{
public:
    static const char *dname;           // "HEQ"
    static const char *var_dname[];     // name of z[i] = {"lambda","Q"} 
    // int IDOF::n;                     // number of variables = 2
    // double *IDOF::z;                 // variables, z = {lambda,Q}
    HEQ();
    HEQ(const HEQ &);
    ~HEQ();
    IDOF *Copy();
 //
    int InitParams(Calc &);
    int ConvertParams(Convert &);
    void PrintParams(std::ostream &);
 //
    std::ostream &Print(std::ostream &, const double *);
    void Load(Calc &, double *);
};

#endif // EOSLIB_HEQ_H
