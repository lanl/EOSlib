#include <EOS.h>
#include "BirchMurnaghan.h"
#include <EOS_VT.h>
#include "BirchMurnaghan_VT.h"

BirchMurnaghan::BirchMurnaghan(BirchMurnaghan_VT &eos_VT)
                    : GenHayes("BirchMurnaghan"), BirchMurnaghanBase(eos_VT)
{
    Transfer(eos_VT);
    
    if( ParamsOK() &&
        !InitODE(BirchMurnaghanParams::eps_abs,BirchMurnaghanParams::eps_rel) )
    {
        EOSstatus = EOSlib::good;
    }
    else
        EOSstatus = EOSlib::abort;
}

EOS_VT *BirchMurnaghan::VT()
{
    return new BirchMurnaghan_VT(*this);
}
