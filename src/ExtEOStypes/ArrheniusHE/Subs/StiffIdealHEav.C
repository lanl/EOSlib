#include "StiffIdealHEav.h"
#include "AvArrheniusIDOF.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;


const char *StiffIdealHEav_lib_vers = StiffIdealHEav_vers;
const char *StiffIdealHEav_lib_date = StiffIdealHEav_date;


extern "C" { // DataBase functions
  char *EOS_StiffIdealHEav_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());        
      }
      if( strcmp(StiffIdealHEav_vers, StiffIdealHEav_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + StiffIdealHEav_lib_vers + "), derived("
                  + StiffIdealHEav_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *EOS_StiffIdealHEav_Info()
  {
      std::string msg(StiffIdealHEav_lib_vers);
      msg = msg + " : " + StiffIdealHEav_lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *EOS_StiffIdealHEav_Constructor()
  {
      EOS *eos = new StiffIdealHEav;
      return static_cast<void*>(eos);
  }
}

StiffIdealHEav::StiffIdealHEav()
    : ExtEOS("StiffIdealHEav"), AvArrheniusRate(static_cast<ExtEOS &>(*this))
{
    z_ref  = new AvArrheniusIDOF;
    HE = NULL;
}

StiffIdealHEav::~StiffIdealHEav()
{
    EOS *eos = HE;
    deleteEOS(eos);
}

ExtEOS *StiffIdealHEav::Copy()
{
    ExtEOS *eos = new StiffIdealHEav(*this);
    return eos;
}

StiffIdealHEav::StiffIdealHEav(const StiffIdealHEav& eos)
    : ExtEOS(eos), AvArrheniusRate(static_cast<const AvArrheniusRate&>(eos))
{
    if( eos.HE == NULL )
    {
        HE = NULL;
        EOSstatus = EOSlib::abort;        
    }
    else
        HE = new StiffIdeal(*eos.HE);
}
EOS *StiffIdealHEav::reactants()
{
    if( HE==NULL )
        return NULL;
    StiffIdealHEav *eos = new StiffIdealHEav(*this);
    eos->set_lambda(0.);
    eos->lambda_ref(0.);    
    return eos;
}
EOS *StiffIdealHEav::products()
{
    if( HE==NULL )
        return NULL;
    StiffIdealHEav *eos = new StiffIdealHEav(*this);
    eos->set_lambda(1.);
    eos->lambda_ref(1.);    
    return eos;
}

void StiffIdealHEav::PrintParams(ostream &out)
{
    ExtEOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
    if( HE )
        out << "\tHE = \""  << HE->Type()
                << "::" << HE->Name() << "\"\n";
    AvArrheniusRate::PrintParams(out);
	out.setf(old, ios::adjustfield);    
}

ostream &StiffIdealHEav::PrintComponents(ostream &out)
{
    if( HE )
        HE->Print(out);
    return out;
}

int StiffIdealHEav::ConvertParams(Convert &convert)
{
    if( HE == NULL || HE->ConvertUnits(convert)
                   ||  AvArrheniusRate::ConvertParams(convert) )
        return 1;
    return 0;   
}

int StiffIdealHEav::PreInit(Calc &calc)
{
    EOS *eos = static_cast<EOS*>(HE);
    deleteEOS(eos);     HE = NULL;
    char *HEname = NULL;
    calc.Variable( "HE", HEname );
    AvArrheniusRate::InitParams(calc);
    return 0;    
}

#define FUNC "StiffIdealHEav::PostInit",__FILE__,__LINE__,this
int StiffIdealHEav::PostInit(Calc &calc, DataBase *db)
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
    if( AvArrheniusRate::ParamsOK() )
    {
        EOSerror->Log(FUNC, "AvArrheniusRate::ParamsOK failed\n" );
        return 1;
    }
    if( std::isnan(V_ref) )
        V_ref = HE->V_ref;
    if( std::isnan(e_ref) )
        e_ref = HE->e_ref;  
    if( (*z_ref)[1] <= 0.0 )
    {
        set_lambda((*z_ref)[0]);
        (*z_ref)[1] = HE->T(V_ref,e_ref);
    }
    return 0;
}
//
int StiffIdealHEav::var(const char *name, double V, double e, const double *z,
                         double &value)
{
/**************    
    if( !strcmp(name,"t_induction") )
    {
        value = t_induction(V,e,z[0]);
        return std::isnan(value);
    }
***************/
    return ExtEOS::var(name,V,e,z,value);    
}
