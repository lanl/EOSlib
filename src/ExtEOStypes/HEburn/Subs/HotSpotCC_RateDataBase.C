#include "HotSpotCC_Rate.h"

#include <string>
#include <cstring>

const char *HotSpotCC_Rate_lib_vers = HotSpotCC_Rate_vers;
const char *HotSpotCC_Rate_lib_date = HotSpotCC_Rate_date;

extern "C" { // DataBase functions
  char *HErate_HotSpotCC_Rate_Init()
  {
      if( HErate_lib_mismatch(HErate_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HErate_lib_vers + "), derived(" + HErate_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(HotSpotCC_Rate_vers, HotSpotCC_Rate_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HotSpotCC_Rate_lib_vers + "), derived("
                  + HotSpotCC_Rate_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *HErate_HotSpotCC_Rate_Info()
  {
      std::string msg(HotSpotCC_Rate_lib_vers);
      msg = msg + " : " + HotSpotCC_Rate_lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *HErate_HotSpotCC_Rate_Constructor()
  {
      HErate *rate = new HotSpotCC_Rate;
      return static_cast<void*>(rate);
  }
}
