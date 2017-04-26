#include <EOS.h>
#include "Hayes.h"

#include <string>
#include <cstring>

const char *Hayeslib_vers  = Hayes_vers;
const char *Hayeslib_date  = Hayes_date;

extern "C" { // dynamic link for database
  char *EOS_Hayes_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_Hayes_Info()
  {
      std::string msg(Hayeslib_vers);
      msg = msg + " : " + Hayeslib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_Hayes_Constructor()
  {
    EOS *eos = new Hayes;
	return static_cast<void *>(eos);
  }
}
