#include "BKW_VT.h"

#include <string>

const char *BKW_VTlib_vers  = BKW_VT_vers;
const char *BKW_VTlib_date  = BKW_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_BKW_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_BKW_Info()
  {
      std::string msg(BKW_VTlib_vers);
      msg = msg + " : " + BKW_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_BKW_Constructor()
  {
	EOS_VT *eos = new BKW_VT;
	return static_cast<void*>(eos);
  }
}
