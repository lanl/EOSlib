#include "EOS.h"
#include <string>

const char *EOSlib_vers  = EOS_vers;
const char *EOSlib_date  = EOS_date;

extern "C" { // dynamic link for database
  char *EOS_Init()
  {
      EOS::Init();
      return NULL;
  }
  
  char *EOS_Info()
  {
      std::string msg(EOSlib_vers);
      msg = msg + " : " + EOSlib_date
                + " : " + BugReport
                + "\n";
      return strdup(msg.c_str());
  }

  int  EOS_Initialize(void *ptr, Parameters &p, DataBase &db)
  {
      EOS *eos = static_cast<EOS*>(ptr);
      return eos->Initialize(p, &db, NULL);
  }
  
  void *EOS_Duplicate(void *ptr)
  {
      EOS *eos = static_cast<EOS*>(ptr);
      eos = eos->Duplicate();
      return static_cast<void *>(eos);
  }
      
  void EOS_Destructor(void *ptr)
  {
      EOS *eos = static_cast<EOS*>(ptr);
      deleteEOS(eos);
  }
}
