#include "HotSpotRate2.h"

#include <string>
#include <cstring>

const char *HotSpotRate2_lib_vers = HotSpotRate2_vers;
const char *HotSpotRate2_lib_date = HotSpotRate2_date;

extern "C" { // DataBase functions
  char *HErate_HotSpotRate2_Init()
  {
      if( HErate_lib_mismatch(HErate_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HErate_lib_vers + "), derived(" + HErate_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(HotSpotRate2_vers, HotSpotRate2_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HotSpotRate2_lib_vers + "), derived("
                  + HotSpotRate2_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *HErate_HotSpotRate2_Info()
  {
      std::string msg(HotSpotRate2_lib_vers);
      msg = msg + " : " + HotSpotRate2_lib_date
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *HErate_HotSpotRate2_Constructor()
  {
      HErate *rate = new HotSpotRate2;
      return static_cast<void*>(rate);
  }
}
