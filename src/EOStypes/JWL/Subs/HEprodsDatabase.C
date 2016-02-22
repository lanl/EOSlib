#include "HEprods.h"

#include <string>

const char *HEprodslib_vers  = HEprods_vers;
const char *HEprodslib_date  = HEprods_date;

extern "C" { // dynamic link for database
  char *EOS_HEprods_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_HEprods_Info()
  {
      std::string msg(HEprodslib_vers);
      msg = msg + " : " + HEprodslib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_HEprods_Constructor()
  {
    EOS *eos = new HEprods;
	return static_cast<void *>(eos);
  }
}
