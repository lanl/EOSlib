#include <EOS_VT.h>
#include "StiffenedGas_VT.h"

#include <string>
#include <cstring>

const char *StiffenedGas_VTlib_vers  = StiffenedGas_VT_vers;
const char *StiffenedGas_VTlib_date  = StiffenedGas_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_StiffenedGas_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_StiffenedGas_Info()
  {
      std::string msg(StiffenedGas_VTlib_vers);
      msg = msg + " : " + StiffenedGas_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_StiffenedGas_Constructor()
  {
	  EOS_VT *eos = new StiffenedGas_VT;
      return static_cast<void *>(eos);
  }
}
