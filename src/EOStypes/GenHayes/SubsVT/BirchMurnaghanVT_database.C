#include <EOS_VT.h>
#include "BirchMurnaghan_VT.h"

#include <string>

const char *BirchMurnaghan_VTlib_vers  = BirchMurnaghan_VT_vers;
const char *BirchMurnaghan_VTlib_date  = BirchMurnaghan_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_BirchMurnaghan_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_BirchMurnaghan_Info()
  {
      std::string msg(BirchMurnaghan_VTlib_vers);
      msg = msg + " : " + BirchMurnaghan_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_BirchMurnaghan_Constructor()
  {
    EOS_VT *eos = new BirchMurnaghan_VT;
	return static_cast<void*>(eos);
  }
}
