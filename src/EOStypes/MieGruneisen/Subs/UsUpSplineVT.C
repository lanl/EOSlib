#include "UsUpSpline_VT.h"
#include "UsUpSpline.h"

#include <string>

const char *UsUpSpline_VTlib_vers  = UsUpSpline_VT_vers;
const char *UsUpSpline_VTlib_date  = UsUpSpline_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_UsUpSpline_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_UsUpSpline_Info()
  {
      std::string msg(UsUpSpline_VTlib_vers);
      msg = msg + " : " + UsUpSpline_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_UsUpSpline_Constructor()
  {
	EOS_VT *eos = new UsUpSpline_VT;
	return static_cast<void*>(eos);
  }
}

UsUpSpline_VT::UsUpSpline_VT() : EOS_VT("UsUpSpline")
{
    // Null
}

UsUpSpline_VT::UsUpSpline_VT(UsUpSpline &eos) : EOS_VT("UsUpSpline")
{
    Transfer(eos);
    if( UsUpSplineParams::copy((const UsUpSplineParams &)eos) )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

UsUpSpline_VT::~UsUpSpline_VT()
{
	// Null
}

EOS *UsUpSpline_VT::Ve()
{
    return new UsUpSpline(*this);
}

void UsUpSpline_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
    UsUpSplineParams::PrintParams(out);
}

int UsUpSpline_VT::ConvertParams(Convert &convert)
{
    int status = EOS_VT::ConvertParams(convert);
    if( status )
        return status;
	return UsUpSplineParams::ConvertParams(convert, EOSerror);
}

#define FUNC "UsUpSpline_VT::InitParams",__FILE__,__LINE__,this
int UsUpSpline_VT::InitParams(Parameters &p, Calc &calc, DataBase *)
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

    if( isnan(V_ref) )
        V_ref = V0;
    if( isnan(T_ref) )
        T_ref = T0;
    return 0;
}
