#include "StiffIdealHE.h"
#include "ArrheniusIDOF.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;


const char *StiffIdealHE_lib_vers = StiffIdealHE_vers;
const char *StiffIdealHE_lib_date = StiffIdealHE_date;


extern "C" { // DataBase functions
  char *EOS_StiffIdealHE_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(StiffIdealHE_vers, StiffIdealHE_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + StiffIdealHE_lib_vers + "), derived("
                  + StiffIdealHE_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *EOS_StiffIdealHE_Info()
  {
      std::string msg(StiffIdealHE_lib_vers);
      msg = msg + " : " + StiffIdealHE_lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *EOS_StiffIdealHE_Constructor()
  {
      EOS *eos = new StiffIdealHE;
      return static_cast<void*>(eos);
  }
}

StiffIdealHE::StiffIdealHE()
    : ExtEOS("StiffIdealHE"), ArrheniusRate(static_cast<ExtEOS &>(*this))
{
    z_ref  = new ArrheniusIDOF;
    HE = NULL;
}

StiffIdealHE::~StiffIdealHE()
{
    EOS *eos = HE;
    deleteEOS(eos);
}

ExtEOS *StiffIdealHE::Copy()
{
    ExtEOS *eos = new StiffIdealHE(*this);
    return eos;
}

StiffIdealHE::StiffIdealHE(const StiffIdealHE& eos)
    : ExtEOS(eos), ArrheniusRate(static_cast<const ArrheniusRate&>(eos))
{
    if( eos.HE == NULL )
    {
        HE = NULL;
        EOSstatus = EOSlib::abort;        
    }
    else
        HE = new StiffIdeal(*eos.HE);
}
EOS *StiffIdealHE::reactants()
{
    if( HE==NULL )
        return NULL;
    StiffIdealHE *eos = new StiffIdealHE(*this);
    eos->set_lambda(0.);
    eos->lambda_ref(0.);    
    return eos;
}
EOS *StiffIdealHE::products()
{
    if( HE==NULL )
        return NULL;
    StiffIdealHE *eos = new StiffIdealHE(*this);
    eos->set_lambda(1.);
    eos->lambda_ref(1.);    
    return eos;
}

void StiffIdealHE::PrintParams(ostream &out)
{
    ExtEOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
    if( HE )
        out << "\tHE = \""  << HE->Type()
                << "::" << HE->Name() << "\"\n";
    ArrheniusRate::PrintParams(out);
	out.setf(old, ios::adjustfield);    
}

ostream &StiffIdealHE::PrintComponents(ostream &out)
{
    if( HE )
        HE->Print(out);
    return out;
}

int StiffIdealHE::ConvertParams(Convert &convert)
{
    if( HE == NULL || HE->ConvertUnits(convert)
                   || ArrheniusRate::ConvertParams(convert) )
        return 1;
    return 0;   
}

int StiffIdealHE::PreInit(Calc &calc)
{
    EOS *eos = static_cast<EOS*>(HE);
    deleteEOS(eos);     HE = NULL;
    char *HEname = NULL;
    calc.Variable( "HE", HEname );
    ArrheniusRate::InitParams(calc);
    return 0;    
}

#define FUNC "StiffIdealHE::PostInit",__FILE__,__LINE__,this
int StiffIdealHE::PostInit(Calc &calc, DataBase *db)
{
    const char *HEname;
    calc.Fetch("HE",HEname);
    if( HEname == NULL )
    {
        EOSerror->Log(FUNC, "HE not specified\n" );
        return 1;
    }
    EOS *eos = FetchEOS("StiffIdeal",HEname,*db);
    if( eos == NULL )
    {
        EOSerror->Log(FUNC, "HE not specified\n" );
        return 1;
    }
    HE = static_cast<StiffIdeal *>(eos); 
    if( ArrheniusRate::ParamsOK() )
    {
        EOSerror->Log(FUNC, "ArrheniusRate::ParamsOK failed\n" );
        return 1;
    }    
    if( std::isnan(V_ref) )
        V_ref = HE->V_ref;
    if( std::isnan(e_ref) )
        e_ref = HE->e_ref;  
    return 0;
}
//
int StiffIdealHE::var(const char *name, double V, double e, const double *z,
                         double &value)
{
    return ExtEOS::var(name,V,e,z,value);    
}
