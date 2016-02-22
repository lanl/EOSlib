#include <EOS.h>
#include "Keane.h"
#include <EOS_VT.h>
#include "Keane_VT.h"

Keane_VT::Keane_VT(Keane &eos_Ve)
                    : GenHayes_VT("Keane"), KeaneBase(eos_Ve)
{
    Transfer(eos_Ve);

    if( ParamsOK() &&
        !InitODE(KeaneParams::eps_abs,KeaneParams::eps_rel) )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

EOS *Keane_VT::Ve()
{
    return new Keane(*this);
}
