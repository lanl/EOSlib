#ifndef EOSLIB_HOTSPOT_HE_H
#define EOSLIB_HOTSPOT_HE_H
#include <IDOF.h>
#include <Calc.h>
//
//  IDOF for reaction model base on shock state
//  z[0] = Ps,      shock pressure
//  z[1] = time-ts, ts = shock arrival time
//  z[2] = lambda,  reaction progress variable (mass fraction of products)
//         0 = reactants (unburnt)
//         1 = products (burnt)
//  z[3] = s,       scaled reaction length
//
class HotSpotHE : public IDOF
{
public:
    static const char *dname;           // "HotSpotHE"
    static const char *var_dname[];     // var names = {"Ps","ts","lambda","s"}
    // int IDOF::n;                     // number of variables = 4
    // double *IDOF::z;                 // variables, z = {Ps,ts,lambda,s}
    HotSpotHE();
    HotSpotHE(const HotSpotHE &);
    ~HotSpotHE();
    IDOF *Copy();
 //
    int InitParams(Calc &);
    int ConvertParams(Convert &);
    void PrintParams(std::ostream &);
 //
    std::ostream &Print(std::ostream &, const double *);
    void Load(Calc &, double *);
};

#endif // EOSLIB_HOTSPOT_HE_H
