#include <EOS.h>
#include "StiffenedGas.h"
#include <EOS_VT.h>
#include "StiffenedGas_VT.h"

StiffenedGas::StiffenedGas(StiffenedGas_VT &gas) : EOS("StiffenedGas")
{
    Transfer(gas);
    *static_cast<StiffGasParams *>(this) = gas;

    if( ParamsOK() )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

EOS_VT *StiffenedGas::VT()
{
    return new StiffenedGas_VT(*this);
}
