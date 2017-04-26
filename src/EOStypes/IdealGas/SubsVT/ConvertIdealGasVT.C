#include <EOS_VT.h>
#include "IdealGas_VT.h"

#define FUNC "IdealGas_VT::ConvertParams"
int IdealGas_VT::ConvertParams(Convert &convert)
{
    int status = EOS_VT::ConvertParams(convert);
    if( status )
        return status;
	if( !finite(Cv *= convert.factor("Cv")) )
	{
	    EOSerror->Log(FUNC, __FILE__, __LINE__, this, "failed\n");
	    return 1;
	}
	return 0;
}
