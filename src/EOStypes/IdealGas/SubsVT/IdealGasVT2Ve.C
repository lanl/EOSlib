#include <EOS.h>
#include "IdealGas.h"
#include <EOS_VT.h>
#include "IdealGas_VT.h"

IdealGas_VT::IdealGas_VT(IdealGas &gas) : EOS_VT("IdealGas")
{
    Transfer(gas);

    gamma = gas.gamma;
    Cv = gas.Cv;

    if( finite(gamma) && finite(Cv) && finite(V_ref) && finite(T_ref) )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

EOS *IdealGas_VT::Ve()
{
    return new IdealGas(*this);
}

