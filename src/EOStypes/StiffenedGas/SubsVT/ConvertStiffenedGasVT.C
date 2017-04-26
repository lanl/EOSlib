#include <EOS_VT.h>
#include "StiffenedGas_VT.h"

#define FUNC "StiffenedGas_VT::ConvertParams"
int StiffenedGas_VT::ConvertParams(Convert &convert)
{
    int status = EOS_VT::ConvertParams(convert);
    if( status )
        return status;
	status = StiffGasParams::ConvertParams(convert);
    if( status )
	    EOSerror->Log(FUNC, __FILE__, __LINE__, this, "failed\n");
    return status;        
}
