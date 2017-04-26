#include "Porous.h"

#include <string>
#include <cstring>


extern "C" { // dynamic link for database
  char *EOS_FrozenPorous_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_FrozenPorous_Info()
  {
      std::string msg(PorousEOSlib_vers);
      msg = msg + " : " + PorousEOSlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());      
  }

  void *EOS_FrozenPorous_Constructor()
  {
      EOS *eos = new FrozenPorous;
	  return static_cast<void*>(eos);
  }
}
