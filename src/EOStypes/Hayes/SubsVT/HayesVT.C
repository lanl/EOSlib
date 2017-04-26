#include <EOS_VT.h>
#include "Hayes_VT.h"

#include <string>
#include <cstring>

const char *Hayes_VTlib_vers  = Hayes_VT_vers;
const char *Hayes_VTlib_date  = Hayes_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_Hayes_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_Hayes_Info()
  {
      std::string msg(Hayes_VTlib_vers);
      msg = msg + " : " + Hayes_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_Hayes_Constructor()
  {
	EOS_VT *eos = new Hayes_VT;
	return static_cast<void*>(eos);
  }
}

Hayes_VT::Hayes_VT() : EOS_VT("Hayes")
{
    // Null
}


Hayes_VT::~Hayes_VT()
{
	// Null
}
