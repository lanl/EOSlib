#include <EOS_VT.h>
#include "HayesBM_VT.h"

#define FUNC "HayesBM_VT::ConvertParams"
int HayesBM_VT::ConvertParams(Convert &convert)
{
    int status = EOS_VT::ConvertParams(convert);
    if( status )
        return status;
	status = HayesBMparams::ConvertParams(convert);
	if( status)
	    EOSerror->Log(FUNC, __FILE__, __LINE__, this, "failed\n");
	return status;
}

