#include "ArrheniusHE.h"

Isentrope *ArrheniusHE::isentrope(const HydroState &state)
{
    if( set_lambda((*z_ref)(0)) )
        return NULL;
    return HE->isentrope(state);
}

Hugoniot *ArrheniusHE::shock(const HydroState &state)
{
    if( set_lambda((*z_ref)(0)) )
        return NULL;
    return HE->shock(state);
}

Isotherm *ArrheniusHE::isotherm(const HydroState &state)
{
    if( set_lambda((*z_ref)(0)) )
        return NULL;
    return HE->isotherm(state);
}

Detonation *ArrheniusHE::detonation(const HydroState &state, double P0)
{
    if( HE == NULL )
        return NULL;
    return HE->eos2->detonation(state,P0);
}

Deflagration *ArrheniusHE::deflagration(const HydroState &state, double P0)
{
    if( HE == NULL )
        return NULL;
    return HE->eos2->deflagration(state,P0);
}
