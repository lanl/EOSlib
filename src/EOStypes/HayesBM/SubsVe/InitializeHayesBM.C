#include <EOS.h>
#include "HayesBM.h"

#define FUNC "HayesBM::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int HayesBM::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    HayesBMparams::InitParams(calc);
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( !ParamsOK() )
    {
        EOSerror->Log(FUNC,"variable not set: V0=%lf, e0=%lf, P0=%lf, T0=%lf\n"
              "\tK0=%lf, Kp0=%lf, G0=%lf, G1=%lf, Cv=%lf\n",
              V0, e0, P0, T0, K0, Kp0, G0, G1, Cv);
        return 1;
    }

    if( isnan(V_ref) )
        V_ref = V0;
    if( isnan(e_ref) )
        e_ref = e0;
    return 0;
}
