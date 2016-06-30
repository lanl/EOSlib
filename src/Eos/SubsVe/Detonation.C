#include "EOS.h"


Detonation::Detonation(EOS &gas, double pvac) : Wave(gas,pvac)
{
    P0=EOS::NaN;
}

Detonation::~Detonation()
{
    // NULL
}

int Detonation::Initialize(const HydroState &state)
{
    eos->ErrorHandler()->Log("Detonation::Initialize",
                 __FILE__, __LINE__, eos, "P0 required\n");
    P0=EOS::NaN;
    return 1;
}

int Detonation::Initialize(const HydroState &state, double p0)
{
    if( eos->InDomain(state.V,state.e) )
    {
        if( eos->P(state) <= p0 )
        {
            P0 = EOS::NaN;
            eos->ErrorHandler()->Log("Detonation::Initialize",
                 __FILE__, __LINE__, eos, "P(state) <= P0\n");
            return 1;
        }
    }
    P0 = p0;
    V0 = state.V;
    e0 = state.e;
    u0 = state.u;
    
    (HydroState &) state0 = state;
    state0.P = P0;
    state0.T = EOS::NaN;
    state0.c = EOS::NaN;

    return InitCJ();
}

int Detonation::InitState(HydroState &state, double &p0)
{
    if( std::isnan(P0) )
        return 1;
    state.V = V0;
    state.e = e0;
    state.u = u0;
    p0 = P0;
    return 0;
}

int Detonation::CJwave(int dir, WaveState &wave)
{
    if( std::isnan(P0) )
        return 1;
    wave.V  = Vcj;
    wave.e  = ecj;
    wave.P  = Pcj;
    wave.u  = u0 + dir*ucj;
    wave.us = u0 + dir*Dcj;
    return 0; 
}
