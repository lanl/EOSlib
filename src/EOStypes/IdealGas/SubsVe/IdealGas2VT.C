#include <EOS.h>
#include "IdealGas.h"
#include <EOS_VT.h>
#include "IdealGas_VT.h"

IdealGas::IdealGas(IdealGas_VT &gas) : EOS("IdealGas")
{
    Transfer(gas);

    gamma = gas.gamma;
    Cv = gas.Cv;

    if( finite(gamma) && finite(Cv) && finite(V_ref) && finite(e_ref) )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

EOS_VT *IdealGas::VT()
{
    return new IdealGas_VT(*this);
}
