#include <EOS.h>
#include "StiffenedGas.h"

#define FUNC "StiffenedGas::ConvertParams"
int StiffenedGas::ConvertParams(Convert &convert)
{
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
	status = StiffGasParams::ConvertParams(convert);
    if( status )
	    EOSerror->Log(FUNC, __FILE__, __LINE__, this, "failed\n");
    return status;        
}
