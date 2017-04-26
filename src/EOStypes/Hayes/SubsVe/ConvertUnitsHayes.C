#include <EOS.h>
#include "Hayes.h"

#define FUNC "Hayes::ConvertParams"
int Hayes::ConvertParams(Convert &convert)
{
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
	status = HayesParams::ConvertParams(convert);
	if( status)
	    EOSerror->Log(FUNC, __FILE__, __LINE__, this, "failed\n");
	return status;
}
