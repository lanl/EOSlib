#include <EOS_VT.h>
#include "Hayes_VT.h"

#define FUNC "Hayes_VT::ConvertParams"
int Hayes_VT::ConvertParams(Convert &convert)
{
    int status = EOS_VT::ConvertParams(convert);
    if( status )
        return status;
	status = HayesParams::ConvertParams(convert);
	if( status)
	    EOSerror->Log(FUNC, __FILE__, __LINE__, this, "failed\n");
	return status;
}

