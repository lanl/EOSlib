#include <EOS_VT.h>
#include "IdealGas_VT.h"


#define FUNC "IdealGas_VT::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int IdealGas_VT::InitParams(Parameters &p, Calc &calc,  DataBase *)
{
    gamma = Cv = NaN;
	calcvar(gamma);
	calcvar(Cv);

	ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( !finite(gamma) || !finite(Cv) )
    {
        EOSerror->Log(FUNC,"variable not set: gamma=%lf, Cv=%lf\n", gamma, Cv);
        return 1;
    }
  	
	return 0;
}
