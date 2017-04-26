#ifndef EOSLIB_RIEMANN_GENX_H
#define EOSLIB_RIEMANN_GENX_H

#include <Riemann_gen.h>

class RiemannSolver_genX
{
private:
    RiemannSolver_generic *RS;
    RiemannSolver_genX();                                   // disallowed
    RiemannSolver_genX(const RiemannSolver_genX&);          // disallowed
    void operator=(const RiemannSolver_genX&);              // disallowed
public:
    RiemannSolver_genX(ExtEOS &left,  const double *zleft,
                       ExtEOS &right, const double *zright);
    ~RiemannSolver_genX()    { delete RS;          }
    double abs_tol() const   {return RS->abs_tol();}
    double rel_tol() const   {return RS->rel_tol();}
    void   abs_tol(double x) { RS->abs_tol(x);     }
    void   rel_tol(double x) { RS->rel_tol(x);     }
    //    
    int Solve(const HydroState &state_l, const HydroState &state_r,
                    WaveState &left,     WaveState &right)
        { return RS ? RS->Solve(state_l,state_r, left,right) : -1;}
};

#endif // EOSLIB_RIEMANN_GENX_H
