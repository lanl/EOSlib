#ifndef EOSLIB_HOTSPOT_HE_CC
#define EOSLIB_HOTSPOT_HE_CC
#include <IDOF.h>
#include <Calc.h>
//
//  IDOF for SURF-plus model (hot spot + carbon clusters)
//  reactants -> intermediate products -> equilibrium products
//  reaction progress variables lambda  for first  reaction
//                          and lambda2 for second reaction
//  z[0] = Ps,      shock pressure
//  z[1] = time-ts, ts = shock arrival time
//  z[2] = s,        lambda = g(s)
//  z[3] = s2,       lambda2 = s2**2
//
class HotSpotHE_CC : public IDOF
{
public:
    static const char *dname;       // "HotSpotHE_CC"
    static const char *var_dname[]; // var names = {"Ps","ts","s", "s2"}
    // int IDOF::n;                 // number of variables = 4
    // double *IDOF::z;             // variables, z = {Ps,ts,s,s2}
    HotSpotHE_CC();
    HotSpotHE_CC(const HotSpotHE_CC &);
    ~HotSpotHE_CC();
    IDOF *Copy();
 //
    int InitParams(Calc &);
    int ConvertParams(Convert &);
    void PrintParams(std::ostream &);
 //
    std::ostream &Print(std::ostream &, const double *);
    void Load(Calc &, double *);
};

#endif // EOSLIB_HOTSPOT_HE_CC
