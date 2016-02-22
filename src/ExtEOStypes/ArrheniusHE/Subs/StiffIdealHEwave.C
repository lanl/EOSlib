#include "StiffIdealHE.h"

Isentrope *StiffIdealHE::isentrope(const HydroState &state)
{
    if( set_lambda((*z_ref)(0)) )
        return NULL;
    return HE->isentrope(state);
}

Hugoniot *StiffIdealHE::shock(const HydroState &state)
{
    if( set_lambda((*z_ref)(0)) )
        return NULL;
    return HE->shock(state);
}

Isotherm *StiffIdealHE::isotherm(const HydroState &state)
{
    if( set_lambda((*z_ref)(0)) )
        return NULL;
    return HE->isotherm(state);
}

Detonation *StiffIdealHE::detonation(const HydroState &state, double P0)
{
    if( HE == NULL )
        return NULL;
    P0 = P(state.V,state.e);
    StiffIdeal *SI = new StiffIdeal(*HE);    
    SI->set_lambda2(1.);
    Detonation *Det = SI->detonation(state,P0);
    EOS *eos = SI;
    deleteEOS(eos);
    return Det;
}

Deflagration *StiffIdealHE::deflagration(const HydroState &state, double P0)
{
    if( HE == NULL )
        return NULL;
    P0 = P(state.V,state.e);
    StiffIdeal *SI = new StiffIdeal(*HE);    
    SI->set_lambda2(1.);
    Deflagration *Def = SI->deflagration(state,P0);
    EOS *eos = SI;
    deleteEOS(eos);
    return Def;
}
