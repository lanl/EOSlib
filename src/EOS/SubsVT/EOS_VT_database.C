#include "EOS_VT.h"
#include <string>
#include <cstring>

const char *EOS_VTlib_vers  = EOS_VT_vers;
const char *EOS_VTlib_date  = EOS_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_Init()
  {
      EOS_VT::Init();
      return NULL;
  }
  
  char *EOS_VT_Info()
  {
      std::string msg(EOS_VTlib_vers);
      msg = msg + " : " + EOS_VTlib_date
                + " : " + BugReport
                + "\n";
      return strdup(msg.c_str());
  }

  int  EOS_VT_Initialize(void *ptr, Parameters &p, DataBase &db)
  {
      return ((EOS_VT *)ptr)->Initialize(p, &db, NULL);
  }
  
  void *EOS_VT_Duplicate(void *ptr)
  {
      return (void *)((EOS_VT *)ptr)->Duplicate();
  }
      
  void EOS_VT_Destructor(void *ptr)
  {
      EOS_VT *eptr = (EOS_VT *)ptr;
      deleteEOS_VT(eptr);
  }
}
