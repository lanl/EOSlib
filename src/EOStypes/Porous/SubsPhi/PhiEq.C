#include "PhiEq.h"

#include <string>
#include <cstring>


const char *PhiEqlib_vers  = PhiEq_vers;
const char *PhiEqlib_date  = PhiEq_date;
//
//
int PhiEq::init = 0;
//
void PhiEq::Init()
{
    if( init )
        return;
    Initbase();
    init = 1;
}


extern "C" { // dynamic link for database
  char *PhiEq_Init()
  {
      PhiEq::Init();
      return NULL;
  }

  char *PhiEq_Info()
  {
      std::string msg(PhiEqlib_vers);
      msg = msg + " : " + PhiEqlib_date
                + " : " + BugReport
                + "\n";
      return strdup(msg.c_str());
  }

  int  PhiEq_Initialize(void *ptr, Parameters &p, DataBase &db)
  {
      PhiEq *PE = static_cast<PhiEq*>(ptr);
      return PE->Initialize(p, &db);
  }
  
  void *PhiEq_Duplicate(void *ptr)
  {
      PhiEq *PE = static_cast<PhiEq*>(ptr);
      PE = PE->Duplicate();
      return static_cast<void *>(PE);
  }
      
  void PhiEq_Destructor(void *ptr)
  {
      PhiEq *PE = static_cast<PhiEq*>(ptr);
      deletePhiEq(PE);
  }
}
//
//
PhiEq::PhiEq(const char *ptype ) : EOSbase("PhiEq",ptype)
{
    if( !init )
        Init();   
 // to be over written by InitPV() of derived class
    phi_min = phi_max = NaN;
    PV_min  = PV_max  = NaN;
    Pmin    = e_max   = NaN;
}

PhiEq::~PhiEq()
{
    // Null
}


void  PhiEq::PrintParams(ostream &out)
{
    out << "\t# phi_min = " << phi_min
                            << ", \tphi_max = " << phi_max << "\n"
        << "\t#  PV_min = " << PV_min
                            << ", \t PV_max = " <<  PV_max << "\n"
        << "\t#    Pmin = " << Pmin
                            << ", \t  e_max = " <<   e_max << "\n";
}

int PhiEq::ConvertParams(Convert &convert)
{
    return 1;   
}

int PhiEq::InitBase(Parameters &p, Calc &calc, DataBase *db)
{
    if( InitParams(p, calc, db) )
    {
        EOSerror->Log("PhiEq::Initialize", __FILE__, __LINE__, this,
                      "Initialize for %s failed\n", type);
        return 1;
    }
    return 0;
}
