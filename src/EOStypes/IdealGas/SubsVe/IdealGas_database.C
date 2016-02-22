#include <EOS.h>
#include "IdealGas.h"

#include <string>
#include <cstring>

const char *IdealGaslib_vers  = IdealGas_vers;
const char *IdealGaslib_date  = IdealGas_date;

extern "C" { // dynamic link for database
  char *EOS_IdealGas_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_IdealGas_Info()
  {
      std::string msg(IdealGaslib_vers);
      msg = msg + " : " + IdealGaslib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_IdealGas_Constructor()
  {
    EOS *eos = new IdealGas;
	return static_cast<void *>(eos);
  }
}
