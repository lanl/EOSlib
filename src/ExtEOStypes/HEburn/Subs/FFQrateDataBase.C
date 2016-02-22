#include "FFQrate.h"

#include <string>
#include <cstring>

const char *FFQrate_lib_vers = FFQrate_vers;
const char *FFQrate_lib_date = FFQrate_date;

extern "C" { // DataBase functions
  char *HErate_FFQrate_Init()
  {
      if( HErate_lib_mismatch(HErate_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HErate_lib_vers + "), derived(" + HErate_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(FFQrate_vers, FFQrate_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + FFQrate_lib_vers + "), derived(" + FFQrate_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  char *HErate_FFQrate_Info()
  {
      std::string msg(FFQrate_lib_vers);
      msg = msg + " : " + FFQrate_lib_date 
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *HErate_FFQrate_Constructor()
  {
      HErate *rate = new FFQrate;
      return static_cast<void*>(rate);
  }
}
