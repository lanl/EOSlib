#include "PeSpline.h"
#include "PeSpline_VT.h"

#include <string>

const char *PeSplinelib_vers  = PeSpline_vers;
const char *PeSplinelib_date  = PeSpline_date;

extern "C" { // dynamic link for database
  char *EOS_PeSpline_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_PeSpline_Info()
  {
      std::string msg(PeSplinelib_vers);
      msg = msg + " : " + PeSplinelib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_PeSpline_Constructor()
  {
    EOS *eos = new PeSpline;
	return static_cast<void *>(eos);
  }
}

//

PeSpline::PeSpline() : EOS("PeSpline")
{
    // Null
}

PeSpline::PeSpline(PeSpline_VT &eos) : EOS("PeSpline")
{
    Transfer(eos);
    if( PeSplineParams::copy((const PeSplineParams &)eos) )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

PeSpline::~PeSpline()
{
    // Null
}

EOS_VT *PeSpline::VT()
{
    return new PeSpline_VT(*this);
}

void PeSpline::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    PeSplineParams::PrintParams(out);
}

int PeSpline::ConvertParams(Convert &convert)
{
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
	return PeSplineParams::ConvertParams(convert, EOSerror);
}

#define FUNC "PeSpline::InitParams",__FILE__,__LINE__,this
int PeSpline::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    PeSplineParams::InitParams(calc);
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( PeSplineParams::ParamsOK(calc, EOSerror) )
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
