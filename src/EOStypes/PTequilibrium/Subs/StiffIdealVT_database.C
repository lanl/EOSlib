#include "StiffIdeal_VT.h"
#include "StiffIdeal.h"

#include <string>
#include <cstring>

const char *StiffIdeal_VTlib_vers  = StiffIdeal_VT_vers;
const char *StiffIdeal_VTlib_date  = StiffIdeal_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_StiffIdeal_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_StiffIdeal_Info()
  {
      std::string msg(StiffIdeal_VTlib_vers);
      msg = msg + " : " + StiffIdeal_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());      
  }
  
  void *EOS_VT_StiffIdeal_Constructor()
  {
	EOS_VT *eos = new StiffIdeal_VT;
	return static_cast<void*>(eos);
  }
}
