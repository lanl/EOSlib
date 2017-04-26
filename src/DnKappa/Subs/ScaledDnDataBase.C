#include <string>
#include <cstring>

#include "ScaledDn.h"

const char *ScaledDn_lib_vers = ScaledDn_vers;
const char *ScaledDn_lib_date = ScaledDn_date;

extern "C" { // DataBase functions
  char *DnKappa_ScaledDn_Init()
  {
      if( DnKappa_lib_mismatch(DnKappa_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + DnKappa_lib_vers + "), derived(" + DnKappa_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(ScaledDn_vers, ScaledDn_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + ScaledDn_lib_vers + "), derived(" + ScaledDn_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  char *DnKappa_ScaledDn_Info()
  {
      std::string msg(ScaledDn_lib_vers);
      msg = msg + " : " + ScaledDn_lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *DnKappa_ScaledDn_Constructor()
  {
      ScaledDn *SDn = new ScaledDn;
      return static_cast<void*>(SDn);
  }
}
