#include <EOS.h>
#include "BirchMurnaghan.h"

#include <string>
#include <cstring>


const char *BirchMurnaghanlib_vers  = BirchMurnaghan_vers;
const char *BirchMurnaghanlib_date  = BirchMurnaghan_date;

extern "C" { // dynamic link for database
  char *EOS_BirchMurnaghan_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_BirchMurnaghan_Info()
  {
      std::string msg(BirchMurnaghanlib_vers);
      msg = msg + " : " + BirchMurnaghanlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_BirchMurnaghan_Constructor()
  {
    EOS *eos = new BirchMurnaghan;
	return static_cast<void*>(eos);
  }
}
