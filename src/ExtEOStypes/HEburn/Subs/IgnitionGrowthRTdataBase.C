#include "IgnitionGrowthRT.h"

#include <string>
#include <cstring>


const char *IgnitionGrowthRT_lib_vers = IgnitionGrowthRT_vers;
const char *IgnitionGrowthRT_lib_date = IgnitionGrowthRT_date;

extern "C" { // DataBase functions
  char *HErate_IgnitionGrowthRT_Init()
  {
      if( HErate_lib_mismatch(HErate_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HErate_lib_vers + "), derived(" + HErate_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(IgnitionGrowthRT_vers, IgnitionGrowthRT_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + IgnitionGrowthRT_lib_vers + "), derived("
                  + IgnitionGrowthRT_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *HErate_IgnitionGrowthRT_Info()
  {
      std::string msg(IgnitionGrowthRT_lib_vers);
      msg = msg + " : " + IgnitionGrowthRT_lib_date
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *HErate_IgnitionGrowthRT_Constructor()
  {
      HErate *rate = new IgnitionGrowthRT;
      return static_cast<void*>(rate);
  }
}
