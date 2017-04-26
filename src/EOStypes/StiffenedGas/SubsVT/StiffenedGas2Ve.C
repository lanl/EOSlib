#include <EOS.h>
#include "StiffenedGas.h"
#include <EOS_VT.h>
#include "StiffenedGas_VT.h"

StiffenedGas_VT::StiffenedGas_VT(StiffenedGas &gas) : EOS_VT("StiffenedGas")
{
    Transfer(gas);
    *static_cast<StiffGasParams *>(this) = gas;

    if( ParamsOK() )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

EOS *StiffenedGas_VT::Ve()
{
    return new StiffenedGas(*this);
}
