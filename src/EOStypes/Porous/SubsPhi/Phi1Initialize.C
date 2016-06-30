#include "PhiTypes.h"

#define FUNC "Phi1::InitParams", __FILE__, __LINE__, this
#define calcvar(var) calc.Variable(#var,&var)
int Phi1::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    P0 = V0 = PV0 = PV1 = phi0 = NaN;
    dphidP0 = dphidP1 = NaN;
    phi1 = 1.;
    n = -1;
    a = NaN;
    sphi0 = 0;
    sPV1  = 1;
    abs_tol = rel_tol = 1e-4;
    
    calcvar(V0);
    calcvar(P0);
    calcvar(PV0);
    calcvar(PV1);
    calcvar(phi0);
    calcvar(phi1);
    calcvar(n);
    calcvar(a);
    calcvar(dphidP0);
    calcvar(dphidP1);
    calcvar(sphi0);
    calcvar(sPV1);
    calcvar(abs_tol);
    calcvar(rel_tol);

    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC, "parse failed for %s\n", line );
        return 1;
    }
    
    if( std::isnan(V0) || isnan(P0) || isnan(PV1) || isnan(phi0)
        || std::isnan(dphidP0) || isnan(dphidP1) || isnan(a) || n < 0 )
    {
        EOSerror->Log(FUNC, "variable not set: V0=%lf, P0=%lf, PV1=%lf, phi0=%lf\n"
                "\tdphidP0=%lf, dphidP1=%lf, a=%lf, n=%d\n",
               V0, P0, PV1, phi0, dphidP0, dphidP1, a, n );
        return 1;
    }
    if( std::isnan(PV0) )
        PV0 = P0*V0;

    sPV1 *= PV1;
        
    if( InitPV() )
    {
        EOSerror->Log(FUNC, "InitPV() failed\n" );
        return 1;
    }
    
    return 0;
}

