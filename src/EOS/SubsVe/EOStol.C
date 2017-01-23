#include "EOS.h"

EOS_tol::EOS_tol() : dV_factor(1.e-3),        de_factor(1.e-2),
                     c2_tol(1.e-3),           Gamma_tol(1.e-3),
                     CV_tol(1.e-3),           FD_tol(1.e-3),
                     OneDFunc_rel_tol(1.e-8), OneDFunc_abs_tol(1.e-14),
                     ODE_epsilon(1.e-8),      Iter_max(20),
                     P_vac(1e-6)
{
    // Null
}

int EOS_tol::Convert_tol(Convert &convert)
{
    double Pfact = convert.factor("P");
    if( !finite(Pfact) )
        return 1;
    P_vac *= Pfact;
    return 0;    
}

#define calcvar(var) calc.Variable(#var,&var)
void EOS_tol::InitParams(Calc &calc)
{
    calcvar(dV_factor);
    calcvar(de_factor);
    calcvar(c2_tol);
    calcvar(Gamma_tol);
    calcvar(CV_tol);
    calcvar(FD_tol);
    calcvar(OneDFunc_abs_tol);
    calcvar(OneDFunc_rel_tol);
    calcvar(ODE_epsilon);
    calcvar(Iter_max);
    calcvar(P_vac);
}
