#include "PhiTypes.h"

#include <string>
#include <cstring>

extern "C" { // dynamic link for database
  char *PhiEq_Phi0_Init()
  {
      if( PhiEqlib_mismatch(PhiEq_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + PhiEqlib_vers + "), derived(" + PhiEq_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *PhiEq_Phi0_Info()
  {
      std::string msg(PhiEqlib_vers);
      msg = msg + " : " + PhiEqlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *PhiEq_Phi0_Constructor()
  {
      PhiEq *phi = new Phi0;
      return static_cast<void*>(phi);
  }
}

Phi0::Phi0() : PhiEq_spline("Phi0"), scale(0)
{
    abs_tol = 1e-4;
    rel_tol = 1e-4;
    P0 = V0 = phi0 = Pc = NaN;
}


Phi0::~Phi0()
{
    // Null
}

double Phi0::f(double PV)
{
    double r = (PV - P0*V0)/(Pc*V0);
    return 1. -(1.-phi0)*exp(-r);
}

int Phi0::InitPV(double pv_0, double pv_1)
{    
    PhiEq::Pmin = P0;
    return PhiEq_spline::InitPV(pv_0, pv_1);
}
