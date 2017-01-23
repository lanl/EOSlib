#ifndef EOSLIB_DETONATION_H
#define EOSLIB_DETONATION_H

#include <ODE.h>

class Detonation_gen : public Detonation, private ODE
{
    friend class Detonation_gen_CJ;
    friend class Detonation_gen_u2;
    friend class Detonation_gen_us;
    friend class Detonation_gen_weak_us;
private:
    void operator=(const Detonation_gen&);      // disallowed
    Detonation_gen(const Detonation_gen&);      // disallowed
    Detonation_gen *operator &();               // disallowed
protected:
//  double Detonation::{P0, V0, e0, u0};
//  double Detonation::{Pcj, Dcj, Vcj, ucj, ecj};
    int InitCJ();
    int F(double *y_prime, const double *y, double V); // ODE::F
    double e[1];
public:
    double rel_tol;                             // tolerance for near CJ state
    Detonation_gen(EOS &e, double pvac = EOS::NaN);
    ~Detonation_gen();

    int P  (double p,  int direction, WaveState &shock);   
    int u  (double u,  int direction, WaveState &shock);   
    int u_s(double us, int direction, WaveState &shock);
    int w_u_s(double us, int direction, WaveState &shock);
    int V  (double v,  int direction, WaveState &shock);
};

#endif // EOSLIB_DETONATION_H
