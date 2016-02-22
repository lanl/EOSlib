#include "IgnitionGrowth.h"

#include <string>
#include <cstring>


const char *IgnitionGrowth_lib_vers = IgnitionGrowth_vers;
const char *IgnitionGrowth_lib_date = IgnitionGrowth_date;

extern "C" { // DataBase functions
  char *HErate_IgnitionGrowth_Init()
  {
      if( HErate_lib_mismatch(HErate_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HErate_lib_vers + "), derived(" + HErate_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(IgnitionGrowth_vers, IgnitionGrowth_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + IgnitionGrowth_lib_vers + "), derived("
                  + IgnitionGrowth_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *HErate_IgnitionGrowth_Info()
  {
      std::string msg(IgnitionGrowth_lib_vers);
      msg = msg + " : " + IgnitionGrowth_lib_date
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *HErate_IgnitionGrowth_Constructor()
  {
      HErate *rate = new IgnitionGrowth;
      return static_cast<void*>(rate);
  }
}
