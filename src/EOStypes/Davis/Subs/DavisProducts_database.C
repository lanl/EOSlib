#include "DavisProducts.h"
#include <string>

const char *DavisProductslib_vers  = DavisProducts_vers;
const char *DavisProductslib_date  = DavisProducts_date;

extern "C" { // dynamic link for database
  char *EOS_DavisProducts_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_DavisProducts_Info()
  {
      std::string msg(DavisProductslib_vers);
      msg = msg + " : " + DavisProductslib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_DavisProducts_Constructor()
  {
    EOS *eos = new DavisProducts;
	return static_cast<void *>(eos);
  }
}
