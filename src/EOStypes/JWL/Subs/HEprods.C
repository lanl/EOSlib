#include "HEprods.h"

HEprods::HEprods() : EOS("HEprods")
{
    V0= 0.0;
    e0= 0.0;
    P0= 0.0;

    rho_cj = 0.0;
      e_cj = 0.0;
      P_cj = 0.0;

    rho_min = 0.0;
    rho_max = 0.0;

    rho_sw = 0.0;
    T_sw = 0.0;
    gamma1 = 0.0;
    gamma2 = 0.0;
    na1 = 0;
    na2 = 0;
    nb1 = 0;
    nb2 = 0;

    Cv   = 0.0;
    T_cj = 0.0;
    S_cj = 0.0;
}

HEprods::~HEprods()
{
	// Null
}
