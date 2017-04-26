#include "BKW.h"

#include <string>

const char *BKWlib_vers  = BKW_vers;
const char *BKWlib_date  = BKW_date;

extern "C" { // dynamic link for database
  char *EOS_BKW_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_BKW_Info()
  {
      std::string msg(BKWlib_vers);
      msg = msg + " : " + BKWlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_BKW_Constructor()
  {
    EOS *eos = new BKW;
	return static_cast<void *>(eos);
  }
}
