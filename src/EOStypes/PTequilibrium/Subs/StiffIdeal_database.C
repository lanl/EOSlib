#include "StiffIdeal.h"
#include "StiffIdeal_VT.h"

#include <string>
#include <cstring>

const char *StiffIdeallib_vers  = StiffIdeal_vers;
const char *StiffIdeallib_date  = StiffIdeal_date;

extern "C" { // dynamic link for database
  char *EOS_StiffIdeal_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_StiffIdeal_Info()
  {
      std::string msg(StiffIdeallib_vers);
      msg = msg + " : " + StiffIdeallib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());      
  }

  void *EOS_StiffIdeal_Constructor()
  {
    EOS *eos = new StiffIdeal;
	return static_cast<void *>(eos);
  }
}
