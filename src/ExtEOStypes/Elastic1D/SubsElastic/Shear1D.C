#include "Shear1D.h"

#include <string>
#include <cstring>


const char *Shear1Dlib_vers = Shear1D_vers;
const char *Shear1Dlib_date = Shear1D_date;

extern "C" { // dynamic link for database
  char *Shear1D_Init()
  {
      return NULL;
  }
  
  char *Shear1D_Info()
  {
      std::string msg(Shear1Dlib_vers);
      msg = msg + " : " + Shear1Dlib_date
                + " : " + BugReport
                + "\n";
      return strdup(msg.c_str());
  }

  int  Shear1D_Initialize(void *ptr, Parameters &p, DataBase &db)
  {
      Shear1D *shear = static_cast<Shear1D*>(ptr);
      return shear->Initialize(p, &db, NULL);
  }
  
  void *Shear1D_Duplicate(void *ptr)
  {
      Shear1D *shear = static_cast<Shear1D*>(ptr);
      shear = shear->Duplicate();
      return static_cast<void *>(shear);
  }
      
  void Shear1D_Destructor(void *ptr)
  {
      Shear1D *shear = static_cast<Shear1D*>(ptr);
      deleteShear1D(shear);
  }
}


Shear1D::Shear1D(const char *stype) : EOSbase("Shear1D",stype)
{
        // Null
}

Shear1D::~Shear1D()
{
        // Null
}

#define FUNC "Shear1D::InitBase", __FILE__, __LINE__, this
int Shear1D::InitBase(Parameters &p, Calc &calc, DataBase *db)
{
    if( InitParams(p, calc, db) )
    {
        EOSstatus = EOSlib::bad;
        EOSerror->Log(FUNC,"Initialize for %s failed\n", type);
        return 1;
    }
    EOSstatus = EOSlib::good;
    return 0;    
}
