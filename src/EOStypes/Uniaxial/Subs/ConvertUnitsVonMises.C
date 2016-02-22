#include <EOS.h>
#include "Uniaxial.h"

#define FUNC "VonMisesElasticPlastic::ConvertParams",__FILE__, __LINE__, this
int VonMisesElasticPlastic::ConvertParams(Convert &convert)
{
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
    double s_e;
	double s_V;
	if( !finite(s_e = convert.factor("e"))
         || !finite(s_V = convert.factor("V")) )
	{
	    EOSerror->Log(FUNC, "failed\n");
	    return 1;
	}
	
	Vy_comp *= s_V;
	Vy_exp  *= s_V;
	
	G *= s_e/s_V;
	Y *= s_e/s_V;

	return UniaxialEOS::ConvertParams(convert);
}

