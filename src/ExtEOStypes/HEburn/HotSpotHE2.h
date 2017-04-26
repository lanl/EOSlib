#ifndef EOSLIB_HOTSPOT_HE2_H
#define EOSLIB_HOTSPOT_HE2_H
#include <IDOF.h>
#include <Calc.h>
//
//  IDOF for reaction model base on shock state
//  z[0] = Ps,      shock pressure
//  z[1] = Ts,      shock termperature
//  z[2] = time-ts, ts = shock arrival time
//  z[3] = lambda,  reaction progress variable (mass fraction of products)
//         0 = reactants (unburnt)
//         1 = products (burnt)
//  z[4] = s,       scaled reaction length
//  z[5] = s0,      (d/dt)s at shock front
//
class HotSpotHE2 : public IDOF
{
public:
    static const char *dname;         // "HotSpotHE"
    static const char *var_dname[];   // var names = {"Ps","Ts","ts","lambda",
                                      //                "s","s0"}
    // int IDOF::n;                   // number of variables = 6
    // double *IDOF::z;               // variables, z = {Ps,Ts,ts,lambda,s.s0}
    HotSpotHE2();
    HotSpotHE2(const HotSpotHE2 &);
    ~HotSpotHE2();
    IDOF *Copy();
 //
    int InitParams(Calc &);
    int ConvertParams(Convert &);
    void PrintParams(std::ostream &);
 //
    std::ostream &Print(std::ostream &, const double *);
    void Load(Calc &, double *);
};

#endif // EOSLIB_HOTSPOT_HE2_H
