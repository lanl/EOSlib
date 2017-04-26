#include <EOS.h>
#include "IdealGas.h"

#define FUNC "IdealGas::ConvertParams"
int IdealGas::ConvertParams(Convert &convert)
{
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
	if( !finite(Cv *= convert.factor("Cv")) )
	{
	    EOSerror->Log(FUNC, __FILE__, __LINE__, this, "failed\n");
	    return 1;
	}
	return 0;
}
