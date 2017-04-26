#ifndef EOSLIB_HOTSPOT_CC_HE_H
#define EOSLIB_HOTSPOT_CC_HE_H
#include <IDOF.h>
#include <Calc.h>
//
//  IDOF for SURF-plus model (hot spot + carbon clusters)
//  z[0] = Ps,      shock pressure
//  z[1] = time-ts, ts = shock arrival time
//  z[2] = lambda,  hot spot reaction progress variable
//         0 = reactants (unburnt)
//         1 = products (burnt)
//  z[3] = s,        scaled reaction length
//  z[4] = s2,       lambda2**(1/2)
//                   lambda2 = carbon cluster reaction progress variable
//  z[5] = e_cc,     carbon cluster specific energy
//
class HotSpotCC_HE : public IDOF
{
public:
    static const char *dname;         // "HotSpotCC_HE"
    static const char *var_dname[];   // var names = {"Ps","ts","lambda","s",
                                      //                        "s2","e_cc"}
    // int IDOF::n;             // number of variables = 6
    // double *IDOF::z;         // variables, z = {Ps,ts,lambda,s,s2,e_cc}
    HotSpotCC_HE();
    HotSpotCC_HE(const HotSpotCC_HE &);
    ~HotSpotCC_HE();
    IDOF *Copy();
 //
    int InitParams(Calc &);
    int ConvertParams(Convert &);
    void PrintParams(std::ostream &);
 //
    std::ostream &Print(std::ostream &, const double *);
    void Load(Calc &, double *);
};

#endif // EOSLIB_HOTSPOT_CC_HE_H
