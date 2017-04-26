#include "HErate2.h"

#include <string>
#include <cstring>

const char *HErate2_lib_vers  = HErate2_vers;
const char *HErate2_lib_date  = HErate2_date;
//
//
extern "C" { // dynamic link for database
  char *HErate2_Init()
  {
      HErate2::Init();
      return NULL;
  }

  char *HErate2_Info()
  {
      std::string msg(HErate2_lib_vers);
      msg = msg + " : " + HErate2_lib_date
                + " : " + BugReport
                + "\n";
      return strdup(msg.c_str());
  }

  int  HErate2_Initialize(void *ptr, Parameters &p, DataBase &db)
  {
      HErate2 *rate = static_cast<HErate2*>(ptr);
      return rate->Initialize(p, &db);
  }
  
  void *HErate2_Duplicate(void *ptr)
  {
      HErate2 *rate = static_cast<HErate2*>(ptr);
      rate = rate->Duplicate();
      return static_cast<void *>(rate);
  }
      
  void HErate2_Destructor(void *ptr)
  {
      HErate2 *rate = static_cast<HErate2*>(ptr);
      deleteHErate2(rate);
  }
}
