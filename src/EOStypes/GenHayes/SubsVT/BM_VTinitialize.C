#include <EOS_VT.h>
#include "BirchMurnaghan_VT.h"

#define FUNC "BirchMurnaghan_VT::Initialize",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)

int BirchMurnaghan_VT::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    BirchMurnaghanParams::InitParams(calc);
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
              "\tK0=%lf, Kp0=%lf, a=%lf, b=%lf\n",
              "\tcv=%lf, %lf, %lf, %lf\n",
                V0,e0, K0,Kp0, a,b, cv[0],cv[1],cv[2],cv[3] );
        return 1;
    }

    if( InitODE(BirchMurnaghanParams::eps_abs,BirchMurnaghanParams::eps_rel) )
    {
        EOSerror->Log(FUNC,"InitODE failed\n");
        return 1;
    }
    EOSstatus = EOSlib::good;

    if( isnan(V_ref) )
        V_ref = V0;
    if( isnan(T_ref) )
         return 1;
    return 0;
}
