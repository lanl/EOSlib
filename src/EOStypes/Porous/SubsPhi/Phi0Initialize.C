#include "PhiTypes.h"

#define FUNC "Phi0::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int Phi0::InitParams(Parameters &p, Calc &calc, DataBase *db)
{  
    P0 = V0 = phi0 = Pc = PV_0 = PV_1 = NaN;   
    scale = 0;
    abs_tol = rel_tol = 1e-4;
    
    calcvar(scale);
    calcvar(V0);
    calcvar(P0);
    calcvar(phi0);
    calcvar(Pc);
    calcvar(PV_0);
    calcvar(PV_1);
    calcvar(scale);
    calcvar(abs_tol);
    calcvar(rel_tol);

    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC, "parse failed for %s\n", line );
        return 1;
    }
           
    if( isnan(V0) || isnan(P0) || isnan(phi0) || isnan(Pc) )
    {
        EOSerror->Log(FUNC, "variable not set: V0=%lf, P0=%lf, phi0=%lf, Pc=%lf\n",
               V0, P0, phi0, Pc );
        return 1;
    }

    if( scale )
    {
        P0 *= phi0;
        V0 /= phi0;
    }

    if( isnan(PV_0) )
        PV_0 = P0*V0;
    
    if( isnan(PV_1) )
        PV_1 = 10*Pc*V0;
    
    if( InitPV(PV_0, PV_1) )
    {
        EOSerror->Log(FUNC, "InitPV(PV_0,PV_1) failed\n" );
        return 1;
    }
    
    return 0;
}
