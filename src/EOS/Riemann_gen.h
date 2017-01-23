#ifndef EOSLIB_RIEMANN_GEN_H
#define EOSLIB_RIEMANN_GEN_H

#include <OneDFunction.h>
#include "Riemann.h"

class RiemannSolver_generic : public RiemannSolver, private OneDFunction
{
private:    
    RiemannSolver_generic();                                // disallowed
    RiemannSolver_generic(const RiemannSolver_generic&);    // disallowed
    void operator=(const RiemannSolver_generic&);           // disallowed
    RiemannSolver_generic *operator &();                    // disallowed

    Wave   *Wave_l; 
    Wave   *Wave_r;

    double f(double p);     // OneDFunction::f
    WaveState left_f;
    WaveState right_f;
    
    double Pmin, Pmax;  // bounds
    double P_l, u_l;
    double P_r, u_r;
    double P_c, u_c;    // linearized solution
    int QuadrantI  (const HydroState &state_l, const HydroState &state_r);
    int QuadrantII (const HydroState &state_l, const HydroState &state_r);
    int QuadrantIII(const HydroState &state_l, const HydroState &state_r);
    int QuadrantIV (const HydroState &state_l, const HydroState &state_r);
public:
    RiemannSolver_generic(EOS &left, EOS &right);
    ~RiemannSolver_generic();
    double abs_tol() const   {return abs_tolerance;}
    double rel_tol() const   {return rel_tolerance;}
    void   abs_tol(double x) { abs_tolerance = x;  }
    void   rel_tol(double x) { rel_tolerance = x;  }
    //    
    int Solve(const HydroState &state_l, const HydroState &state_r,
                    WaveState  &left,          WaveState  &right);
};

#endif // EOSLIB_RIEMANN_GEN_H
