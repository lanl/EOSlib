#include "HEburn2.h"

#include <string>
#include <cstring>


const char *HEburn2_lib_vers = HEburn2_vers;
const char *HEburn2_lib_date = HEburn2_date;

extern "C" { // DataBase functions
  char *EOS_HEburn2_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(HEburn2_vers, HEburn2_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HEburn2_lib_vers + "), derived("
                  + HEburn2_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *EOS_HEburn2_Info()
  {
      std::string msg(HEburn2_lib_vers);
      msg = msg + " : " + HEburn2_lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *EOS_HEburn2_Constructor()
  {
      EOS *eos = new HEburn2;
      return static_cast<void*>(eos);
  }
}
