#include "HEburn.h"

#include <string>
#include <cstring>

const char *HEburn_lib_vers = HEburn_vers;
const char *HEburn_lib_date = HEburn_date;

extern "C" { // DataBase functions
  char *EOS_HEburn_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(HEburn_vers, HEburn_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HEburn_lib_vers + "), derived("
                  + HEburn_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *EOS_HEburn_Info()
  {
      std::string msg(HEburn_lib_vers);
      msg = msg + " : " + HEburn_lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *EOS_HEburn_Constructor()
  {
      EOS *eos = new HEburn;
      return static_cast<void*>(eos);
  }
}
