#include "UsUp_VT.h"
#include "UsUp.h"

#include <string>
#include <cstring>


const char *UsUp_VTlib_vers  = UsUp_VT_vers;
const char *UsUp_VTlib_date  = UsUp_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_UsUp_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_UsUp_Info()
  {
      std::string msg(UsUp_VTlib_vers);
      msg = msg + " : " + UsUp_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_UsUp_Constructor()
  {
	EOS_VT *eos = new UsUp_VT;
	return static_cast<void*>(eos);
  }
}

UsUp_VT::UsUp_VT() : EOS_VT("UsUp")
{
    // Null
}

UsUp_VT::UsUp_VT(UsUp &eos) : EOS_VT("UsUp")
{
    Transfer(eos);
    if( UsUpParams::copy((const UsUpParams &)eos) )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

UsUp_VT::~UsUp_VT()
{
	// Null
}

EOS *UsUp_VT::Ve()
{
    return new UsUp(*this);
}

void UsUp_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
    UsUpParams::PrintParams(out);
}

int UsUp_VT::ConvertParams(Convert &convert)
{
    int status = EOS_VT::ConvertParams(convert);
    if( status )
        return status;
	return UsUpParams::ConvertParams(convert, EOSerror);
}

#define FUNC "UsUp_VT::InitParams",__FILE__,__LINE__,this
int UsUp_VT::InitParams(Parameters &p, Calc &calc, DataBase *)
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

    if( isnan(V_ref) )
        V_ref = V0;
    if( isnan(T_ref) )
        T_ref = T0;
    return 0;
}
