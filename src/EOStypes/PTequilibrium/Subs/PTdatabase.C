#include "PTequilibrium.h"
#include "PTequilibrium_VT.h"

#include <string>
#include <cstring>


const char *PTequilibriumlib_vers  = PTequilibrium_vers;
const char *PTequilibriumlib_date  = PTequilibrium_date;

extern "C" { // dynamic link for database
  char *EOS_PTequilibrium_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_PTequilibrium_Info()
  {
      std::string msg(PTequilibriumlib_vers);
      msg = msg + " : " + PTequilibriumlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_PTequilibrium_Constructor()
  {
    EOS *eos = new PTequilibrium;
	return static_cast<void *>(eos);
  }
}

const char *PTequilibrium_VTlib_vers  = PTequilibrium_VT_vers;
const char *PTequilibrium_VTlib_date  = PTequilibrium_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_PTequilibrium_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_PTequilibrium_Info()
  {
      std::string msg(PTequilibrium_VTlib_vers);
      msg = msg + " : " + PTequilibrium_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_PTequilibrium_Constructor()
  {
	EOS_VT *eos = new PTequilibrium_VT;
	return static_cast<void*>(eos);
  }
}
