#include "FFrate.h"

#include <string>
#include <cstring>

const char *FFrate_lib_vers = FFrate_vers;
const char *FFrate_lib_date = FFrate_date;

extern "C" { // DataBase functions
  char *HErate_FFrate_Init()
  {
      if( HErate_lib_mismatch(HErate_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HErate_lib_vers + "), derived(" + HErate_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(FFrate_vers, FFrate_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + FFrate_lib_vers + "), derived(" + HErate_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  char *HErate_FFrate_Info()
  {
      std::string msg(FFrate_lib_vers);
      msg = msg + " : " + FFrate_lib_date 
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *HErate_FFrate_Constructor()
  {
      HErate *rate = new FFrate;
      return static_cast<void*>(rate);
  }
}
