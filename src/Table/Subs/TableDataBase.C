#include "Table.h"
#include <string>
#include <cstring>

const char *Table_lib_vers  = Table_vers;
const char *Table_lib_date  = Table_date;
//
//
int Table::init = 0;

void Table::Init()
{
    if( init )
        return;
    init = 1;
}
//
//
extern "C" { // dynamic link for database
  char *Table_Init()
  {
      Table::Init();
      return NULL;
  }

  char *Table_Info()
  {
      std::string msg(Table_lib_vers);
      msg = msg + " : " + Table_lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  int  Table_Initialize(void *ptr, Parameters &p, DataBase &db)
  {
      Table *tbl = static_cast<Table*>(ptr);
      return tbl->Initialize(p, &db);
  }
  
  void *Table_Duplicate(void *ptr)
  {
      Table *tbl = static_cast<Table*>(ptr);
      tbl = tbl->Duplicate();
      return static_cast<void *>(tbl);
  }
      
  void Table_Destructor(void *ptr)
  {
      Table *tbl = static_cast<Table*>(ptr);
      deleteTable(tbl);
  }
}
