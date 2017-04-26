#include "EOS_VT.h"
#include "IdealGas_VT.h"

#include <string>
#include <cstring>

const char *IdealGas_VTlib_vers  = IdealGas_VT_vers;
const char *IdealGas_VTlib_date  = IdealGas_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_IdealGas_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_IdealGas_Info()
  {
      std::string msg(IdealGas_VTlib_vers);
      msg = msg + " : " + IdealGas_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_IdealGas_Constructor()
  {
    EOS_VT *eos = new IdealGas_VT;
	return static_cast<void *>(eos);
  }
}
