#include "HEburn.h"

Isentrope *HEburn::isentrope(const HydroState &state)
{
    if( z_ref == NULL )
        return NULL;
    EOS *eos = CopyEOS(&(*z_ref));
    if( eos == NULL )
        return NULL;
    Isentrope *wave = eos->isentrope(state);
    deleteEOS(eos);
    return wave;
}

Hugoniot *HEburn::shock(const HydroState &state)
{
    if( z_ref == NULL )
        return NULL;
    EOS *eos = CopyEOS(&(*z_ref));
    if( eos == NULL )
        return NULL;
    Hugoniot *wave = eos->shock(state);
    deleteEOS(eos);
    return wave;
}

Isotherm *HEburn::isotherm(const HydroState &state)
{
    if( z_ref == NULL )
        return NULL;
    EOS *eos = CopyEOS(&(*z_ref));
    if( eos == NULL )
        return NULL;
    Isotherm *wave = eos->isotherm(state);
    deleteEOS(eos);
    return wave;
}

Detonation *HEburn::detonation(const HydroState &state, double P0)
{
    if( HE == NULL || HE->eos2 == NULL )
        return NULL;
    return HE->eos2->detonation(state,P0);
}

Deflagration *HEburn::deflagration(const HydroState &state, double P0)
{
    if( HE == NULL || HE->eos2 == NULL )
        return NULL;
    return HE->eos2->deflagration(state,P0);
}
