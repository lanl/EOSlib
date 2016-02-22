#include <EOS.h>
#include "HayesBM.h"
#include <EOS_VT.h>
#include "HayesBM_VT.h"

HayesBM_VT::HayesBM_VT(HayesBM &gas) : EOS_VT("HayesBM")
{
    Transfer(gas);
    *static_cast<HayesBMparams *>(this) = gas;

    if( ParamsOK() )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

EOS *HayesBM_VT::Ve()
{
    return new HayesBM(*this);
}
