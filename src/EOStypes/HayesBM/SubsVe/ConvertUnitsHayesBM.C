#include <EOS.h>
#include "HayesBM.h"

#define FUNC "HayesBM::ConvertParams"
int HayesBM::ConvertParams(Convert &convert)
{
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
	status = HayesBMparams::ConvertParams(convert);
	if( status)
	    EOSerror->Log(FUNC, __FILE__, __LINE__, this, "failed\n");
	return status;
}
