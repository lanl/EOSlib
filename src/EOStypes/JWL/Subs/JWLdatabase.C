#include "JWL.h"

#include <string>

const char *JWLlib_vers  = JWL_vers;
const char *JWLlib_date  = JWL_date;

extern "C" { // dynamic link for database
  char *EOS_JWL_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_JWL_Info()
  {
      std::string msg(JWLlib_vers);
      msg = msg + " : " + JWLlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_JWL_Constructor()
  {
    EOS *eos = new JWL;
	return static_cast<void *>(eos);
  }
}
