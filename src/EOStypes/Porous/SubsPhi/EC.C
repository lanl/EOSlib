#include "PhiTypes.h"

#include <string>
#include <cstring>


extern "C" { // dynamic link for database
  char *PhiEq_EC_Init()
  {
      if( PhiEqlib_mismatch(PhiEq_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + PhiEqlib_vers + "), derived(" + PhiEq_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *PhiEq_EC_Info()
  {
      std::string msg(PhiEqlib_vers);
      msg = msg + " : " + PhiEqlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *PhiEq_EC_Constructor()
  {
      PhiEq *phi = new EC;
      return static_cast<void*>(phi);
  }
}


EC::EC() : PhiEq_spline("EC"), data(NULL), expt(NULL), hmx(NULL)
{
    // Null
}

EC::~EC()
{
    delete [] hmx;
    delete [] expt;
    delete data;
}


double EC::f(double PV)
{
    return data->Evaluate(PV);
}

ostream &EC::PrintEnergy(ostream &out)
{
    out << "energy(phi_smax, phi_max): "
        << energy(phi_smax) << " " << e_max;

    return out;
}
