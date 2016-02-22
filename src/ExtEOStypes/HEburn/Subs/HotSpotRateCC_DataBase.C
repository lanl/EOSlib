#include "HotSpotRateCC.h"

#include <string>
#include <cstring>

const char *HotSpotRateCC_lib_vers = HotSpotRateCC_vers;
const char *HotSpotRateCC_lib_date = HotSpotRateCC_date;

extern "C" { // DataBase functions
  char *HErate2_HotSpotRateCC_Init()
  {
      if( HErate2_lib_mismatch(HErate2_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HErate2_lib_vers + "), derived(" + HErate2_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(HotSpotRateCC_vers, HotSpotRateCC_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HotSpotRateCC_lib_vers + "), derived("
                  + HotSpotRateCC_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *HErate2_HotSpotRateCC_Info()
  {
      std::string msg(HotSpotRateCC_lib_vers);
      msg = msg + " : " + HotSpotRateCC_lib_date
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *HErate2_HotSpotRateCC_Constructor()
  {
      HErate2 *rate = new HotSpotRateCC;
      return static_cast<void*>(rate);
  }
}
