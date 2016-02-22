#include <EOS_VT.h>
#include "Sesame_VT.h"

#include <string>
#include <cstring>

const char *Sesame_VTlib_vers  = Sesame_VT_vers;
const char *Sesame_VTlib_date  = Sesame_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_Sesame_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_Sesame_Info()
  {
      std::string msg(Sesame_VTlib_vers);
      msg = msg + " : " + Sesame_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_Sesame_Constructor()
  {
	EOS_VT *eos = new Sesame_VT;
	return static_cast<void*>(eos);
  }
}
