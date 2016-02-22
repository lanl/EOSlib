#include "PeSpline_VT.h"
#include "PeSpline.h"

#include <string>

const char *PeSpline_VTlib_vers  = PeSpline_VT_vers;
const char *PeSpline_VTlib_date  = PeSpline_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_PeSpline_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_PeSpline_Info()
  {
      std::string msg(PeSpline_VTlib_vers);
      msg = msg + " : " + PeSpline_VTlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  
  void *EOS_VT_PeSpline_Constructor()
  {
	EOS_VT *eos = new PeSpline_VT;
	return static_cast<void*>(eos);
  }
}

PeSpline_VT::PeSpline_VT() : EOS_VT("PeSpline")
{
    // Null
}

PeSpline_VT::PeSpline_VT(PeSpline &eos) : EOS_VT("PeSpline")
{
    Transfer(eos);
    if( PeSplineParams::copy((const PeSplineParams &)eos) )
        EOSstatus = EOSlib::good;
    else
        EOSstatus = EOSlib::abort;
}

PeSpline_VT::~PeSpline_VT()
{
	// Null
}

EOS *PeSpline_VT::Ve()
{
    return new PeSpline(*this);
}

void PeSpline_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
    PeSplineParams::PrintParams(out);
}

int PeSpline_VT::ConvertParams(Convert &convert)
{
    int status = EOS_VT::ConvertParams(convert);
    if( status )
        return status;
	return PeSplineParams::ConvertParams(convert, EOSerror);
}

#define FUNC "PeSpline_VT::InitParams",__FILE__,__LINE__,this
int PeSpline_VT::InitParams(Parameters &p, Calc &calc, DataBase *)
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

    if( isnan(V_ref) )
        V_ref = V0;
    if( isnan(T_ref) )
        T_ref = T0;
    return 0;
}
