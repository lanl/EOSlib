#include "EOS.h"
#include "EOSstate.h"

#include <iostream>
#include <iomanip>
using namespace std;

EOSstate::EOSstate(EOS &mat, double VV, double ee, double Up, double Us)
			: V(VV), e(ee), u(Up), us(Us)
{
	eos = mat.Duplicate(); 
	
	P = eos->P(V,e);
	T = eos->T(V,e);
    S = eos->S(V,e);
	c = eos->c(V,e);
}

EOSstate::~EOSstate()
{
	deleteEOS(eos);		
}

ostream &operator<<(ostream &out, EOSstate &state)
{
    if( state.eos == NULL )
        return out;

    out << "eos = " << state.eos->Type() << "::" << state.eos->Name();
    const Units *u = state.eos->UseUnits();
    if( u )
    {
        out << "; units = " << u->Type() << "::" << u->Name();
    }
        
    out << "\n"
        << "  V = " << state.V << "\n"
        << "  e = " << state.e << "\n"
        << "  u = " << state.u << "\n"
        << "  P = " << state.P << "\n"
        << "  T = " << state.T << "\n"
        << "  S = " << state.S << "\n"
        << "  c = " << state.c << "\n";
    if( finite(state.us) )
        out << " us = " << state.us << "\n";
    return out;
}
