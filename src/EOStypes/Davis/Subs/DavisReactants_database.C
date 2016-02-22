#include "DavisReactants.h"
#include <string>

const char *DavisReactantslib_vers  = DavisReactants_vers;
const char *DavisReactantslib_date  = DavisReactants_date;

extern "C" { // dynamic link for database
  char *EOS_DavisReactants_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_DavisReactants_Info()
  {
      std::string msg(DavisReactantslib_vers);
      msg = msg + " : " + DavisReactantslib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_DavisReactants_Constructor()
  {
    EOS *eos = new DavisReactants;
	return static_cast<void *>(eos);
  }
}

