#include <EOS.h>
#include "HayesBM.h"
#include <string>
#include <cstring>


const char *HayesBMlib_vers  = HayesBM_vers;
const char *HayesBMlib_date  = HayesBM_date;

extern "C" { // dynamic link for database
  char *EOS_HayesBM_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_HayesBM_Info()
  {
      std::string msg(HayesBMlib_vers);
      msg = msg + " : " + HayesBMlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_HayesBM_Constructor()
  {
    EOS *eos = new HayesBM;
	return static_cast<void *>(eos);
  }
}

HayesBM::HayesBM() : EOS("HayesBM")
{
	// Null
}

HayesBM::~HayesBM()
{
	// Null
}
