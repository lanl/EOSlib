#include "MatToSesame.h"
#include <string>
#include <cstring>

const char *MatToSesame_lib_vers = MatToSesame_vers;
const char *MatToSesame_lib_date = MatToSesame_date;

extern "C" { // DataBase functions
  char *Table_MatToSesame_Init()
  {
      if( Table_lib_mismatch(Table_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + Table_lib_vers + "), derived(" + Table_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(MatToSesame_vers, MatToSesame_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + MatToSesame_lib_vers
                  + "), derived(" + MatToSesame_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  char *Table_MatToSesame_Info()
  {
      std::string msg(MatToSesame_lib_vers);
      msg = msg + " : " + MatToSesame_lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *Table_MatToSesame_Constructor()
  {
      MatToSesame *tbl = new MatToSesame;
      return static_cast<void*>(tbl);
  }
}
