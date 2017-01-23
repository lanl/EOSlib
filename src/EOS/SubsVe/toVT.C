#include <EOS.h>
#include <EOS_VT.h>

EOS::~EOS()
{
    deleteEOS_VT(VTeos);
}

EOS_VT *EOS::VT()
{
    return NULL;
}

EOS_VT *EOS::eosVT()
{
    if( VTeos == NULL )
        VTeos = VT();
    return VTeos ? VTeos->Duplicate() : NULL;
}

void EOS::Transfer(EOS_VT &eos)
{
    EOSbase::Copy(eos,0);
    V_ref = eos.V_ref;
    e_ref = eos.e(V_ref,eos.T_ref);
}
