#include "HErate.h"

#include <string>
#include <cstring>


const char *HErate_lib_vers  = HErate_vers;
const char *HErate_lib_date  = HErate_date;
//
//
extern "C" { // dynamic link for database
  char *HErate_Init()
  {
      HErate::Init();
      return NULL;
  }

  char *HErate_Info()
  {
      std::string msg(HErate_lib_vers);
      msg = msg + " : " + HErate_lib_date
                + " : " + BugReport
                + "\n";
      return strdup(msg.c_str());
  }

  int  HErate_Initialize(void *ptr, Parameters &p, DataBase &db)
  {
      HErate *rate = static_cast<HErate*>(ptr);
      return rate->Initialize(p, &db);
  }
  
  void *HErate_Duplicate(void *ptr)
  {
      HErate *rate = static_cast<HErate*>(ptr);
      rate = rate->Duplicate();
      return static_cast<void *>(rate);
  }
      
  void HErate_Destructor(void *ptr)
  {
      HErate *rate = static_cast<HErate*>(ptr);
      deleteHErate(rate);
  }
}
