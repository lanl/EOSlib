#include <string>
#include <cstring>

#include "DnKappa.h"

const char *DnKappa_lib_vers  = DnKappa_vers;
const char *DnKappa_lib_date  = DnKappa_date;
//
//
int DnKappa::init = 0;

void DnKappa::Init()
{
    if( init )
        return;
    init = 1;
}
//
//
extern "C" { // dynamic link for database
  char *DnKappa_Init()
  {
      DnKappa::Init();
      return NULL;
  }

  char *DnKappa_Info()
  {
      std::string msg(DnKappa_lib_vers);
      msg = msg + " : " + DnKappa_lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  int  DnKappa_Initialize(void *ptr, Parameters &p, DataBase &db)
  {
      DnKappa *tbl = static_cast<DnKappa*>(ptr);
      return tbl->Initialize(p, &db);
  }
  
  void *DnKappa_Duplicate(void *ptr)
  {
      DnKappa *tbl = static_cast<DnKappa*>(ptr);
      tbl = tbl->Duplicate();
      return static_cast<void *>(tbl);
  }
      
  void DnKappa_Destructor(void *ptr)
  {
      DnKappa *tbl = static_cast<DnKappa*>(ptr);
      deleteDnKappa(tbl);
  }
}
