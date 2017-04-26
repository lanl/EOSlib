#include <EOS.h>
#include "Keane.h"
#include <EOS_VT.h>
#include "Keane_VT.h"

Keane::Keane(Keane_VT &eos_VT)
                    : GenHayes("Keane"), KeaneBase(eos_VT)
{
    Transfer(eos_VT);
    
    if( ParamsOK() &&
        !InitODE(KeaneParams::eps_abs,KeaneParams::eps_rel) )
    {
        EOSstatus = EOSlib::good;
    }
    else
        EOSstatus = EOSlib::abort;
}

EOS_VT *Keane::VT()
{
    return new Keane_VT(*this);
}
