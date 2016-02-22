#include "PnRate.h"

#include <string>
#include <cstring>


const char *PnRate_lib_vers = PnRate_vers;
const char *PnRate_lib_date = PnRate_date;

extern "C" { // DataBase functions
  char *HErate_PnRate_Init()
  {
      if( HErate_lib_mismatch(HErate_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HErate_lib_vers + "), derived(" + HErate_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(PnRate_vers, PnRate_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + PnRate_lib_vers + "), derived(" + PnRate_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  char *HErate_PnRate_Info()
  {
      std::string msg(PnRate_lib_vers);
      msg = msg + " : " + PnRate_lib_date
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *HErate_PnRate_Constructor()
  {
      HErate *rate = new PnRate;
      return static_cast<void*>(rate);
  }
}
