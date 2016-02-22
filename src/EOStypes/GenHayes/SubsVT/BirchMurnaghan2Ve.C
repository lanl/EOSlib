#include <EOS.h>
#include "BirchMurnaghan.h"
#include <EOS_VT.h>
#include "BirchMurnaghan_VT.h"

BirchMurnaghan_VT::BirchMurnaghan_VT(BirchMurnaghan &eos_Ve)
                    : GenHayes_VT("BirchMurnaghan"), BirchMurnaghanBase(eos_Ve)
{
    Transfer(eos_Ve);

    if( ParamsOK() &&
        !InitODE(BirchMurnaghanParams::eps_abs,BirchMurnaghanParams::eps_rel) )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

EOS *BirchMurnaghan_VT::Ve()
{
    return new BirchMurnaghan(*this);
}
