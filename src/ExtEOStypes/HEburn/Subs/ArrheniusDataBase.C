#include "Arrhenius.h"

#include <string>
#include <cstring>


const char *Arrhenius_lib_vers = Arrhenius_vers;
const char *Arrhenius_lib_date = Arrhenius_date;

extern "C" { // DataBase functions
  char *HErate_Arrhenius_Init()
  {
      if( HErate_lib_mismatch(HErate_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + HErate_lib_vers + "), derived(" + HErate_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(Arrhenius_vers, Arrhenius_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + Arrhenius_lib_vers + "), derived(" + Arrhenius_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  char *HErate_Arrhenius_Info()
  {
      std::string msg(Arrhenius_lib_vers);
      msg = msg + " : " + Arrhenius_lib_date 
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *HErate_Arrhenius_Constructor()
  {
      HErate *rate = new Arrhenius;
      return static_cast<void*>(rate);
  }
}
