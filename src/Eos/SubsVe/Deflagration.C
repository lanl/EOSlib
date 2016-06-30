#include "EOS.h"

//#include <float.h>
#include <limits>



Deflagration::Deflagration(EOS &gas, double pvac) : Wave(gas,pvac)
{
    P0=EOS::NaN;
}

Deflagration::~Deflagration()
{
    // NULL
}

int Deflagration::Initialize(const HydroState &state)
{
    eos->ErrorHandler()->Log("Deflagration::Initialize",
                 __FILE__, __LINE__, eos, "P0 required\n");
    P0=EOS::NaN;
    return 1;
}

int Deflagration::Initialize(const HydroState &state, double p0)
{
    if( eos->InDomain(state.V,state.e) )
    {
        if( eos->P(state) <= p0 )
        {
            P0 = EOS::NaN;
            eos->ErrorHandler()->Log("Deflagration::Initialize",
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

int Deflagration::InitState(HydroState &state, double &p0)
{
    if( std::isnan(P0) )
        return 1;
    state.V = V0;
    state.e = e0;
    state.u = u0;
    p0 = P0;
    return 0;
}

int Deflagration::CJwave(int dir, WaveState &wave)
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

int Deflagration::P0wave(int dir, WaveState &wave)
{
    std::numeric_limits<double> Dlimits;     // move to LocalMath
    if( std::isnan(P0) )
        return 1;
    wave.V  = Vp0;
    wave.e  = ep0;
    wave.P  = P0;
    wave.u  = u0;
    //wave.us = (dir==1)? Dlimits.max() : Dlimits.lowest() ; // C++11
    wave.us = (dir==1)? Dlimits.max() : -Dlimits.max() ;
    return 0; 
}
