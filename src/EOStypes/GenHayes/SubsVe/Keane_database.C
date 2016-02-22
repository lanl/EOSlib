#include <EOS.h>
#include "Keane.h"

#include <string>


const char *Keanelib_vers  = Keane_vers;
const char *Keanelib_date  = Keane_date;

extern "C" { // dynamic link for database
  char *EOS_Keane_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_Keane_Info()
  {
      std::string msg(Keanelib_vers);
      msg = msg + " : " + Keanelib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_Keane_Constructor()
  {
    EOS *eos = new Keane;
	return static_cast<void*>(eos);
  }
}
