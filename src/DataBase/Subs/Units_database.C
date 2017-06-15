#include "DataBase.h"
#include <string>

const char *EOSlib_units_vers  = UNITS_vers;
const char *EOSlib_units_date  = UNITS_date;


extern "C" { // dynamic link for database
  char *Units_Units_Init()
  {
      return NULL;
  }

  char *Units_Units_Info()
  {
      std::string msg(EOSlib_units_vers);
      msg = msg + " : " + EOSlib_units_date
                + " : " + BugReport
                + "\n";
      return strdup(msg.c_str());
  }
  
  void *Units_Units_Constructor()
  {
      return new Units;
  }

  int  Units_Initialize(void *ptr, Parameters &p, DataBase &db)
  {
      return ((Units *)ptr)->Initialize(p,db);
  }
  
  void *Units_Duplicate(void *ptr)
  {
      return (void *)((Units *)ptr)->Duplicate();
  }
      
  void Units_Destructor(void *ptr)
  {
      Units *uptr  = (Units *)ptr;
      deleteUnits(uptr);
  }
}
