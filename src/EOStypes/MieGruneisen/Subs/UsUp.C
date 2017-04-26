#include "UsUp.h"
#include "UsUp_VT.h"

#include <string>

const char *UsUplib_vers  = UsUp_vers;
const char *UsUplib_date  = UsUp_date;

extern "C" { // dynamic link for database
  char *EOS_UsUp_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_UsUp_Info()
  {
      std::string msg(UsUplib_vers);
      msg = msg + " : " + UsUplib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_UsUp_Constructor()
  {
    EOS *eos = new UsUp;
	return static_cast<void *>(eos);
  }
}

//

UsUp::UsUp() : EOS("UsUp")
{
    // Null
}

UsUp::UsUp(UsUp_VT &eos) : EOS("UsUp")
{
    Transfer(eos);
    if( UsUpParams::copy((const UsUpParams &)eos) )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

UsUp::~UsUp()
{
    // Null
}

EOS_VT *UsUp::VT()
{
    return new UsUp_VT(*this);
}

void UsUp::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    UsUpParams::PrintParams(out);
}

int UsUp::ConvertParams(Convert &convert)
{
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
	return UsUpParams::ConvertParams(convert, EOSerror);
}

#define FUNC "UsUp::InitParams",__FILE__,__LINE__,this
int UsUp::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    UsUpParams::InitParams(calc);
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( UsUpParams::ParamsOK(EOSerror) )
    {
	    EOSstatus = EOSlib::bad;
	    EOSerror->Log(FUNC, "failed\n");
	    return 1;
    } 
    P_vac = Pmin;
    if( std::isnan(V_ref) )
        V_ref = V0;
    if( std::isnan(e_ref) )
        e_ref = e0;
    return 0;
}
