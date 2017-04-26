#include "PhiEqSpline.h"

#define FUNC "PhiEq_spline::ConvertParams"
int PhiEq_spline::ConvertParams(Convert &convert)
{
	double s;
	double s_p;
	if( !finite(s=convert.factor("e")) 
	     || !finite(s_p=convert.factor("P")) )
	{
	    EOSerror->Log(FUNC, __FILE__, __LINE__, this, "failed\n");
	    return 1;
	}
	s_e *= s;
	
	PV_min  *= s_e;
	PV_max  *= s_e;
	PV_smax *= s_e;
	
	Pmin  *= s_p;
	e_max *= s_e;
    
	return 0;
}
