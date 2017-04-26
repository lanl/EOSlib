#include <EOS_VT.h>
#include "HayesBM_VT.h"
#include <string>
#include <cstring>

const char *HayesBM_VTlib_vers  = HayesBM_VT_vers;
const char *HayesBM_VTlib_date  = HayesBM_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_HayesBM_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_HayesBM_Info()
  {
      std::string msg(HayesBM_VTlib_vers);
      msg = msg + " : " + HayesBM_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_HayesBM_Constructor()
  {
	EOS_VT *eos = new HayesBM_VT;
	return static_cast<void*>(eos);
  }
}

HayesBM_VT::HayesBM_VT() : EOS_VT("HayesBM")
{
    // Null
}


HayesBM_VT::~HayesBM_VT()
{
	// Null
}
