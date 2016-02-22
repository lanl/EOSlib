#include <EOS.h>
#include "StiffenedGas.h"

#include <string>
#include <cstring>

const char *StiffenedGaslib_vers = StiffenedGas_vers;
const char *StiffenedGaslib_date = StiffenedGas_date;

extern "C" { // dynamic link for database
  char *EOS_StiffenedGas_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_StiffenedGas_Info()
  {
      std::string msg(StiffenedGaslib_vers);
      msg = msg + " : " + StiffenedGaslib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_StiffenedGas_Constructor()
  {
    EOS *eos = new StiffenedGas;
	return static_cast<void *>(eos);
  }
}
