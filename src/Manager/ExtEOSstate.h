#ifndef EOSLIB_EXT_EOS_STATE_H
#define EOSLIB_EXT_EOS_STATE_H

#include "ExtEOS.h"

class ExtEOSstate
{
public:
    ExtEOS *xeos;
    double *z;

    double V;
    double e;
    double u;

    double P;
    double T;
    double S;

    double c;
    double us;

    ExtEOSstate() : xeos(NULL), z(NULL) {}
    ExtEOSstate(const ExtEOSstate &s) : xeos(NULL), z(NULL) { *this = s;}
    ExtEOSstate(ExtEOS &mat, const double *zs = NULL);
    ExtEOSstate(ExtEOS &mat, const HydroState&, const double *zs = NULL);
    ExtEOSstate(ExtEOS &mat, const WaveState&,  const double *zs = NULL);
    ~ExtEOSstate();
    void Init(ExtEOS &mat, const double *zs = NULL);
    int N() { xeos ? xeos->N() : 0; }
 //
    void Load(Calc &);
    void Evaluate() {Evaluate(z);}
    void Evaluate(const double *zs);
    void operator=(const HydroState&);
    void operator=(const WaveState&);
    void operator=(const ExtEOSstate&);
    operator HydroState() { return HydroState(V,e,u); }
};

std::ostream &operator<<(std::ostream &out, ExtEOSstate &state);

#endif //  EOSLIB_EXT_EOS_STATE_H
