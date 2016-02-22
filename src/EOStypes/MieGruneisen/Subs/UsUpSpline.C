#include "UsUpSpline.h"
#include "UsUpSpline_VT.h"

#include <string>


const char *UsUpSplinelib_vers  = UsUpSpline_vers;
const char *UsUpSplinelib_date  = UsUpSpline_date;

extern "C" { // dynamic link for database
  char *EOS_UsUpSpline_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_UsUpSpline_Info()
  {
      std::string msg(UsUpSplinelib_vers);
      msg = msg + " : " + UsUpSplinelib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_UsUpSpline_Constructor()
  {
    EOS *eos = new UsUpSpline;
	return static_cast<void *>(eos);
  }
}

//

UsUpSpline::UsUpSpline() : EOS("UsUpSpline")
{
    // Null
}

UsUpSpline::UsUpSpline(UsUpSpline_VT &eos) : EOS("UsUpSpline")
{
    Transfer(eos);
    if( UsUpSplineParams::copy((const UsUpSplineParams &)eos) )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

UsUpSpline::~UsUpSpline()
{
    // Null
}

EOS_VT *UsUpSpline::VT()
{
    return new UsUpSpline_VT(*this);
}

void UsUpSpline::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    UsUpSplineParams::PrintParams(out);
}

int UsUpSpline::ConvertParams(Convert &convert)
{
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
	return UsUpSplineParams::ConvertParams(convert, EOSerror);
}

#define FUNC "UsUpSpline::InitParams",__FILE__,__LINE__,this
int UsUpSpline::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    UsUpSplineParams::InitParams(calc);
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( UsUpSplineParams::ParamsOK(calc, EOSerror) )
    {
	    EOSstatus = EOSlib::bad;
	    EOSerror->Log(FUNC, "failed\n");
	    return 1;
    }        
    P_vac = Pmin;

    if( isnan(V_ref) )
        V_ref = V0;
    if( isnan(e_ref) )
        e_ref = e0;
    return 0;
}
