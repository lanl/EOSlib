#include <EOS.h>
#include "HayesBM.h"
#include <EOS_VT.h>
#include "HayesBM_VT.h"

HayesBM::HayesBM(HayesBM_VT &eos) : EOS("HayesBM")
{
    Transfer(eos);
    *static_cast<HayesBMparams *>(this) = eos;

    if( ParamsOK() )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

EOS_VT *HayesBM::VT()
{
    return new HayesBM_VT(*this);
}
