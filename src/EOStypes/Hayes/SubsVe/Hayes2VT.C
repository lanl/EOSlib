#include <EOS.h>
#include "Hayes.h"
#include <EOS_VT.h>
#include "Hayes_VT.h"

Hayes::Hayes(Hayes_VT &eos) : EOS("Hayes")
{
    Transfer(eos);
    *static_cast<HayesParams *>(this) = eos;

    if( ParamsOK() )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

EOS_VT *Hayes::VT()
{
    return new Hayes_VT(*this);
}
