#include "JWL_VT.h"

#include <string>

const char *JWL_VTlib_vers  = JWL_VT_vers;
const char *JWL_VTlib_date  = JWL_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_JWL_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_JWL_Info()
  {
      std::string msg(JWL_VTlib_vers);
      msg = msg + " : " + JWL_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_JWL_Constructor()
  {
	EOS_VT *eos = new JWL_VT;
	return static_cast<void*>(eos);
  }
}
