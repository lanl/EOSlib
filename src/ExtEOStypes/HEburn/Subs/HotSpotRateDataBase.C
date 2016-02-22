#include "HotSpotRate.h"

#include <string>
#include <cstring>


const char *HotSpotRate_lib_vers = HotSpotRate_vers;
const char *HotSpotRate_lib_date = HotSpotRate_date;

extern "C" { // DataBase functions
  char *HErate_HotSpotRate_Init()
  {
      if( HErate_lib_mismatch(HErate_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HErate_lib_vers + "), derived(" + HErate_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(HotSpotRate_vers, HotSpotRate_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HotSpotRate_lib_vers + "), derived("
                  + HotSpotRate_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *HErate_HotSpotRate_Info()
  {
      std::string msg(HotSpotRate_lib_vers);
      msg = msg + " : " + HotSpotRate_lib_date
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *HErate_HotSpotRate_Constructor()
  {
      HErate *rate = new HotSpotRate;
      return static_cast<void*>(rate);
  }
}
