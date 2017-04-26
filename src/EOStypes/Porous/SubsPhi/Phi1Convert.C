#include "PhiTypes.h"

#define FUNC "Phi1::ConvertParams"
int Phi1::ConvertParams(Convert &convert)
{
    if( PhiEq_spline::ConvertParams(convert) )
        return 1;
    
	double s_V;
	double s_P;
	if( !finite(s_V=convert.factor("V")) 
	     || !finite(s_P=convert.factor("P")) )
	{
	    EOSerror->Log(FUNC, __FILE__, __LINE__, this, "failed\n");
	    return 1;
	}
	V0   *= s_V;
	P0   *= s_P;
	PV0  *= s_P*s_V;
	PV1  *= s_P*s_V;
	sPV1 *= s_P*s_V;   
    return 0;
}
