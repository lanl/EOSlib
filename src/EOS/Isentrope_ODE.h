#ifndef EOSLIB_ISENTROPE_ODE_H
#define EOSLIB_ISENTROPE_ODE_H

#include <ODE.h>

class Isentrope_ODE : public Isentrope, private ODE
{
private:
    void operator=(const Isentrope_ODE&);       // disallowed
    Isentrope_ODE(const Isentrope_ODE&);        // disallowed
    Isentrope_ODE *operator &();                // disallowed

    double MaxNorm(const double *y0, const double *y1,
            const double*, const double*);      // ODE::MaxNorm
public:
    Isentrope_ODE(EOS &e, double pvac = EOS::NaN);
    ~Isentrope_ODE();
    
    int Initialize(const HydroState &state);
    int F(double *yp, const double *y, double V);
// Usage:
//  Initialize(state)
//  status = this->V(v,RIGHT,wave)
    int V(double V, int direction, WaveState &wave);
    int P(double p, int direction, WaveState &wave);
    int u(double u, int direction, WaveState &wave);
    int u_s(double us, int direction, WaveState &wave);
// added functionality, for error recovery
    int LastState(double &V, double *y, double *yp=0)
        { return ODE::LastState(V,y,yp); } // y = {e,u}, yp = {-P, rho*c}
//  single step
    int Forward(double &V, double *y, double *yp);  // next larger V
    int Backward(double &V, double *y, double *yp); // next smaller V   
};

#endif // EOSLIB_ISENTROPE_ODE_H
