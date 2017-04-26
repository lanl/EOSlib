#include "ArrheniusRT.h"

#include <string>
#include <cstring>

const char *ArrheniusRT_lib_vers = ArrheniusRT_vers;
const char *ArrheniusRT_lib_date = ArrheniusRT_date;

extern "C" { // DataBase functions
  char *HErate_ArrheniusRT_Init()
  {
      if( HErate_lib_mismatch(HErate_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HErate_lib_vers + "), derived(" + HErate_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(ArrheniusRT_vers, ArrheniusRT_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + ArrheniusRT_lib_vers + "), derived("
                  + ArrheniusRT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  char *HErate_ArrheniusRT_Info()
  {
      std::string msg(ArrheniusRT_lib_vers);
      msg = msg + " : " + ArrheniusRT_lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *HErate_ArrheniusRT_Constructor()
  {
      HErate *rate = new ArrheniusRT;
      return static_cast<void*>(rate);
  }
}
