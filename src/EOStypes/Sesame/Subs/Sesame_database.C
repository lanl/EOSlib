#include <EOS.h>
#include "Sesame.h"

#include <string>
#include <cstring>

extern "C" { // dynamic link for database
  char *EOS_Sesame_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_Sesame_Info()
  {
      char *msg = strdup("Sesame EOS derived from Sesame_VT");
      return msg;
  }

  void *EOS_Sesame_Constructor()
  {
    EOS *eos = new Sesame;
	return static_cast<void *>(eos);
  }
}
