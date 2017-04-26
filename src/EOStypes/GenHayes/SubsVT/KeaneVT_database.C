#include <EOS_VT.h>
#include "Keane_VT.h"

#include <string>


const char *Keane_VTlib_vers  = Keane_VT_vers;
const char *Keane_VTlib_date  = Keane_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_Keane_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_Keane_Info()
  {
      std::string msg(Keane_VTlib_vers);
      msg = msg + " : " + Keane_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_Keane_Constructor()
  {
    EOS_VT *eos = new Keane_VT;
	return static_cast<void*>(eos);
  }
}
