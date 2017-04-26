#include "PhiTypes.h"

#define FUNC "Phi0::ConvertParams"
int Phi0::ConvertParams(Convert &convert)
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
	Pc   *= s_P;	
	PV_0 *= s_P;
	PV_1 *= s_P;   
	return 0;
}
