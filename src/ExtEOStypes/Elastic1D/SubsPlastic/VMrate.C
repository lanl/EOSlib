#include "VMrate.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;


const char *VMratelib_vers = VMrate_vers;
const char *VMratelib_date = VMrate_date;

extern "C" { // DataBase functions
  char *EOS_VMrate_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      if( strcmp(ElasticPlastic1D_vers, ElasticPlastic1D_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + ElasticPlastic1D_lib_vers + "), derived("
                  + ElasticPlastic1D_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  char *EOS_VMrate_Info()
  {
      std::string msg(VMratelib_vers);
      msg = msg + " : " + VMratelib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *EOS_VMrate_Constructor()
  {
      EOS *eos = new VMrate;
      return static_cast<void*>(eos);
  }
}

VMrate::VMrate() : ElasticPlastic1D("VMrate")
{
    z_ref  = new PlasticStrain2;
    hydro  = NULL;
    PGshear = NULL;
    epsy_el = NaN;
    tau1    = NaN;
      n1    = 1.;
}

VMrate::VMrate(const VMrate &VM) : ElasticPlastic1D(VM)
{
    epsy_el = VM.epsy_el;
    tau1    = VM.tau1;
      n1    = VM.n1;
    if( elastic == NULL )
    {
        hydro  = NULL;
        PGshear = NULL;
    }
    else
    {
        hydro = VM.elastic->hydro;
        PGshear = dynamic_cast<PolyG*>(VM.elastic->shear);
    }  
}

VMrate::~VMrate()
{
    // Null    
}

ExtEOS *VMrate::Copy()
{
    VMrate *eos = new VMrate(*this);
    return eos;
}


void VMrate::PrintParams(ostream &out)
{
    ElasticPlastic1D::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
    out << "\tepsy_el = " << epsy_el << "\n"
        << "\ttau1    = " << tau1    << "\n"
        << "\t  n1    = " << n1      << "\n";
	out.setf(old, ios::adjustfield);  
}

int VMrate::ConvertParams(Convert &convert)
{
    double s_t;
    if( !finite(s_t=convert.factor("time")) )
    {
        EOSerror->Log("VMrate::ConvertParams", __FILE__, __LINE__, this,
                      "failed\n");
        return 1;
    }
    tau1  *= s_t;
    return ElasticPlastic1D::ConvertParams(convert);
}

int VMrate::PreInit(Calc &calc)
{
    tau1 = epsy_el = NaN;
    n1 = 1.;
    calc.Variable("tau1", &tau1);
    calc.Variable("epsy_el", &epsy_el);
    calc.Variable("n1", &n1);    
    hydro   = NULL;
    PGshear   = NULL;
    return ElasticPlastic1D::PreInit(calc);
}

int VMrate::PostInit(Calc &calc, DataBase *db)
{
    if( ElasticPlastic1D::PostInit(calc,db) )
        return 1;
    hydro   = elastic->hydro;
    PGshear = dynamic_cast<PolyG*>(elastic->shear);
    if( PGshear == NULL )
    {
        EOSerror->Log("VMrate::PostInit",__FILE__,__LINE__,this,
                      "shear not type PolyG\n");
        return 1;      
    }
    if( isnan(tau1) || tau1 <= 0. || isnan(epsy_el) || epsy_el < 0.)
    {
        EOSerror->Log("VMrate::PostInit", __FILE__, __LINE__, this,
                      "tau1 or epsy_el not defined or wrong sign\n");
        return 1;      
    }
    return 0;
}
