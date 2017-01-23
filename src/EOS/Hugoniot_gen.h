#ifndef EOSLIB_HUGONIOT_H
#define EOSLIB_HUGONIOT_H

#include <OneDFunction.h>

class Hugoniot_generic : public Hugoniot, private OneDFunction
{
    enum Hugoniot_type
    {
        pressure=0, velocity=1, wave_speed=2, specificV=3
    };
private:
    void operator=(const Hugoniot_generic&);    // disallowed
    Hugoniot_generic(const Hugoniot_generic&);  // disallowed
    Hugoniot_generic *operator &();             // disallowed
protected:
    Isentrope *S;           // eos->isentrope(state0) if needed
    double s;               // 0.5*eos->FD(state0)    if needed

    double f(double);       // OneDFunction::f
    Hugoniot_type type;
    double V1, e1, p1;      // temporaries used by f()
    double p_av, du, m2;
    
    virtual int Vbound_P(double p, double &Vmin, double &fmin,
                           double &Vmax, double &fmax);
    virtual int Vbound_u(double u, double &Vmin, double &fmin,
                           double &Vmax, double &fmax);
    virtual int Vbound_us(double u, double &Vmin, double &fmin,
                            double &Vmax, double &fmax);
    virtual int e_bound(double v, double &e_min, double &fmin,
                           double &e_max, double &fmax);
public:
    Hugoniot_generic(EOS &e, double pvac = EOS::NaN);
    ~Hugoniot_generic();
// Usage:
//  Initialize(state)
//  status = this->P(p,RIGHT,wave)
    int P(double p, int direction, WaveState &shock);   
    int u(double u, int direction, WaveState &shock);   
    int u_s(double us, int direction, WaveState &shock);
    int V(double v, int direction, WaveState &shock);
};

#endif // EOSLIB_HUGONIOT_H
