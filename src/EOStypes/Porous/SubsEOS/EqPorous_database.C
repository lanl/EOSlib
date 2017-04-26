#include "Porous.h"

#include <string>
#include <cstring>


const char *PorousEOSlib_vers  = PorousEOS_vers;
const char *PorousEOSlib_date  = PorousEOS_date;

extern "C" { // dynamic link for database
  char *EOS_EqPorous_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  
  char *EOS_EqPorous_Info()
  {
      std::string msg(PorousEOSlib_vers);
      msg = msg + " : " + PorousEOSlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_EqPorous_Constructor()
  {
      EOS *eos = new EqPorous;
	  return static_cast<void*>(eos);
  }
}
