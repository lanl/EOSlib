#include "IdealHE.h"
#include "ArrheniusIDOF.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;

const char *IdealHE_lib_vers = IdealHE_vers;
const char *IdealHE_lib_date = IdealHE_date;


extern "C" { // DataBase functions
  char *EOS_IdealHE_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());                
      }
      if( strcmp(IdealHE_vers, IdealHE_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + IdealHE_lib_vers + "), derived("
                  + IdealHE_vers + ")\n";
        return strdup(msg.c_str());                
      }
      return NULL;
  }
  char *EOS_IdealHE_Info()
  {
      std::string msg(IdealHE_lib_vers);
      msg = msg + " : " + IdealHE_lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *EOS_IdealHE_Constructor()
  {
      EOS *eos = new IdealHE;
      return static_cast<void*>(eos);
  }
}

IdealHE::IdealHE()
    : ExtEOS("IdealHE"), ArrheniusRate(static_cast<ExtEOS &>(*this))
{
    z_ref  = new ArrheniusIDOF;
    gamma = Cv = Q = NaN;
}

IdealHE::~IdealHE()
{
    // Null
}

ExtEOS *IdealHE::Copy()
{
    ExtEOS *eos = new IdealHE(*this);
    return eos;
}

IdealHE::IdealHE(const IdealHE& eos)
    : ExtEOS(eos), ArrheniusRate(static_cast<const ArrheniusRate&>(eos))
{
    gamma = eos.gamma;
       Cv = eos.Cv;
        Q = eos.Q;
}

EOS *IdealHE::reactants()
{
    double z[1] = {0.0};
    return Copy(z);
}
EOS *IdealHE::products()
{
    double z[1] = {1.0};
    return Copy(z);
}

void IdealHE::PrintParams(ostream &out)
{
    ExtEOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
    out << "\tgamma = " << gamma << "\n"
        << "\t  Cv  = " << Cv    << "\n"
        << "\t  Q   = " << Q     << "\n";
    ArrheniusRate::PrintParams(out);
	out.setf(old, ios::adjustfield);    
}

int IdealHE::ConvertParams(Convert &convert)
{
    if( ArrheniusRate::ConvertParams(convert) )
        return 1;
    double s_T, s_Cv;
    if( !finite(s_T=convert.factor("T"))
        || !finite(s_Cv=convert.factor("Cv")) )
    {
        EOSerror->Log("IdealHE::ConvertParams", __FILE__, __LINE__, this,
                      "failed\n");
        return 1;
    }
    Cv  *= s_Cv;
    Q   *= s_Cv*s_T;
    return 0;   
}

int IdealHE::PreInit(Calc &calc)
{
    gamma = Cv = Q = NaN;
    calc.Variable("gamma", &gamma);
    calc.Variable("Cv",  &Cv);
    calc.Variable("Q",   &Q);
    ArrheniusRate::InitParams(calc);
    return 0;    
}

#define FUNC "IdealHE::PostInit",__FILE__,__LINE__,this
int IdealHE::PostInit(Calc &calc, DataBase *db)
{
  if( std::isnan(gamma) || gamma<=1. || std::isnan(Cv) || Cv<=0.
                     || std::isnan(Q)  || Q<=0. )
    {
        EOSerror->Log(FUNC, "gamma or Cv or Q not defined or wrong sign\n");
        return 1;      
    }
    if( ArrheniusRate::ParamsOK() )
    {
        EOSerror->Log(FUNC, "ArrheniusRate::ParamsOK failed\n" );
        return 1;
    }
    return 0;
}
//
int IdealHE::var(const char *name, double V, double e, const double *z,
                         double &value)
{
    return ExtEOS::var(name,V,e,z,value);    
}
