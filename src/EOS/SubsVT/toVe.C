#include <EOS.h>
#include <EOS_VT.h>

EOS_VT::~EOS_VT()
{
    deleteEOS(Ve_eos);
}

EOS *EOS_VT::Ve()
{
    return NULL;
}

EOS *EOS_VT::eosVe()
{
    if( Ve_eos == NULL )
        Ve_eos = Ve();
    return Ve_eos ? Ve_eos->Duplicate() : NULL;
}

void EOS_VT::Transfer(EOS &eos)
{
    EOSbase::Copy(eos,0);
    V_ref = eos.V_ref;
    T_ref = eos.T(V_ref,eos.e_ref);
}
