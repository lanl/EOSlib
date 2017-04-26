#include <EOS.h>
#include "Hayes.h"
#include <EOS_VT.h>
#include "Hayes_VT.h"

Hayes_VT::Hayes_VT(Hayes &gas) : EOS_VT("Hayes")
{
    Transfer(gas);
    *static_cast<HayesParams *>(this) = gas;

    if( ParamsOK() )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

EOS *Hayes_VT::Ve()
{
    return new Hayes(*this);
}
