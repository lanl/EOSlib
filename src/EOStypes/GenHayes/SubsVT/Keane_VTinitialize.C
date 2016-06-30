#include <EOS_VT.h>
#include "Keane_VT.h"

#define FUNC "Keane_VT::Initialize",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)

int Keane_VT::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    KeaneParams::InitParams(calc);
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    
    if( !ParamsOK() )
    {
        EOSerror->Log(FUNC,"variable not set: V0=%lf, e0=%lf\n"
              "\tK0=%lf, Kp0=%lf, Kpinfty=%lf\n"
              "\ta=%lf, b=%lf\n"
              "\tcv=%lf, %lf, %lf, %lf\n"
              "\tV1=%lf, Kp1=%lf, Kp1infty=%lf\n",
                V0, e0,
                K0, Kp0, Kp0infty,
                a,b, cv[0],cv[1],cv[2],cv[3],
                V1, Kp1, Kp1infty);
        return 1;
    }

    if( InitODE(KeaneParams::eps_abs,KeaneParams::eps_rel) )
    {
        EOSerror->Log(FUNC,"InitODE failed\n");
        return 1;
    }
    EOSstatus = EOSlib::good;

    if( std::isnan(V_ref) )
        V_ref = V0;
    if( std::isnan(T_ref) )
         return 1;
    return 0;
}
