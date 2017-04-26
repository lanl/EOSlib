#include "HotSpotRateExp.h"

#include <string>
#include <cstring>


const char *HotSpotRateExp_lib_vers = HotSpotRateExp_vers;
const char *HotSpotRateExp_lib_date = HotSpotRateExp_date;

extern "C" { // DataBase functions
  char *HErate_HotSpotRateExp_Init()
  {
      if( HErate_lib_mismatch(HErate_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HErate_lib_vers + "), derived(" + HErate_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(HotSpotRateExp_vers, HotSpotRateExp_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HotSpotRateExp_lib_vers + "), derived("
                  + HotSpotRateExp_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *HErate_HotSpotRateExp_Info()
  {
      std::string msg(HotSpotRateExp_lib_vers);
      msg = msg + " : " + HotSpotRateExp_lib_date
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *HErate_HotSpotRateExp_Constructor()
  {
      HErate *rate = new HotSpotRateExp;
      return static_cast<void*>(rate);
  }
}
