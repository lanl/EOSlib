#ifndef EOSLIB_DEFLAGRATION_H
#define EOSLIB_DEFLAGRATION_H

#include <ODE.h>

class Deflagration_gen : public Deflagration, private ODE
{
    friend class Deflagration_gen_CJ;
    friend class Deflagration_gen_u2;
    friend class Deflagration_gen_us;
private:
    void operator=(const Deflagration_gen&);      // disallowed
    Deflagration_gen(const Deflagration_gen&);    // disallowed
    Deflagration_gen *operator &();               // disallowed
protected:
//  double Deflagration::{P0, V0, e0, u0};
//  double Deflagration::{Vp0, ep0};
//  double Deflagration::{Pcj, Dcj, Vcj, ucj, ecj};
    int InitCJ();
    int F(double *y_prime, const double *y, double V); // ODE::F
    double e[1];
public:
    double rel_tol;                             // tolerance for near CJ state
    Deflagration_gen(EOS &e, double pvac = EOS::NaN);
    ~Deflagration_gen();

    int P  (double p,  int direction, WaveState &shock);   
    int u  (double u,  int direction, WaveState &shock);   
    int u_s(double us, int direction, WaveState &shock);
    int V  (double v,  int direction, WaveState &shock);
};

#endif // EOSLIB_DEFLAGRATION_H
