#include "EOS.h"

Wave::Wave(EOS &e, double pvac) : eos(e.Duplicate()), p_vac(pvac)
{
    if( std::isnan(p_vac) )
        p_vac = eos->P_vac;
}

Wave::~Wave()
{
    deleteEOS(eos);
}

int Wave::Initialize(const HydroState &state)
{
    return eos->Evaluate(state,state0);
}

int  Wave::w_u_s(double us, int direction, WaveState &shock)
{
    return 1;  // overwrite for weak detonation only
}
