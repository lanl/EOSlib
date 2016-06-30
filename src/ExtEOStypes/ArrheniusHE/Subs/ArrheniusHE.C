#include "ArrheniusHE.h"
#include "ArrheniusIDOF.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;

const char *ArrheniusHE_lib_vers = ArrheniusHE_vers;
const char *ArrheniusHE_lib_date = ArrheniusHE_date;


extern "C" { // DataBase functions
  char *EOS_ArrheniusHE_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());                
      }
      if( strcmp(ArrheniusHE_vers, ArrheniusHE_lib_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + ArrheniusHE_lib_vers + "), derived("
                  + ArrheniusHE_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  char *EOS_ArrheniusHE_Info()
  {
      std::string msg(ArrheniusHE_lib_vers);
      msg = msg + " : " + ArrheniusHE_lib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }
  void *EOS_ArrheniusHE_Constructor()
  {
      EOS *eos = new ArrheniusHE;
      return static_cast<void*>(eos);
  }
}

ArrheniusHE::ArrheniusHE()
    : ExtEOS("ArrheniusHE"), ArrheniusRate(static_cast<ExtEOS &>(*this))
{
    z_ref  = new ArrheniusIDOF;
    HE = NULL;
}

ArrheniusHE::ArrheniusHE(const ArrheniusHE& eos)
    : ExtEOS(eos), ArrheniusRate(static_cast<const ArrheniusRate&>(eos))
{
    if( eos.HE == NULL )
    {
        HE = NULL;
        EOSstatus = EOSlib::abort;        
    }
    else
        HE = new PTequilibrium(*eos.HE);
}

ArrheniusHE::~ArrheniusHE()
{
    EOS *eos = HE;
    deleteEOS(eos);
}

ExtEOS *ArrheniusHE::Copy()
{
    ExtEOS *eos = new ArrheniusHE(*this);
    return eos;
}

void ArrheniusHE::PrintParams(ostream &out)
{
    ExtEOS::PrintParams(out);
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
    if( HE )
    {
        EOS *eos = HE->eos1;
        if( eos )
            out << "\treactants = \""  << eos->Type()
                << "::" << eos->Name() << "\"\n";
        eos = HE->eos2;
        if( eos )
            out << "\tproducts  = \""  << eos->Type()
                << "::" << eos->Name() << "\"\n";
        out << "\t#tol = " << HE->tol << "\n";
    }
    ArrheniusRate::PrintParams(out);
	out.setf(old, ios::adjustfield);    
}

ostream &ArrheniusHE::PrintComponents(ostream &out)
{
    if( HE )
    {
        EOS *eos = HE->eos1;
        if( eos )
            eos->PrintAll(out);
        eos = HE->eos2;
        if( eos )
            eos->PrintAll(out);
    }
    return out;
}

int ArrheniusHE::ConvertParams(Convert &convert)
{
    if( HE == NULL || HE->ConvertUnits(convert)
                   || ArrheniusRate::ConvertParams(convert) )
        return 1;
    return 0;   
}

int ArrheniusHE::PreInit(Calc &calc)
{
    EOS *eos = static_cast<EOS*>(HE);
    deleteEOS(eos);     HE = NULL;
    char *reactants = NULL;
    char *products  = NULL;
    calc.Variable( "reactants", reactants );
    calc.Variable( "products", products );
    ArrheniusRate::InitParams(calc);
    return 0;    
}

#define FUNC "ArrheniusHE::PostInit",__FILE__,__LINE__,this
int ArrheniusHE::PostInit(Calc &calc, DataBase *db)
{
    const char *reactants;
    calc.Fetch("reactants",reactants);
    if( reactants == NULL )
    {
        EOSerror->Log(FUNC, "reactants not specified\n" );
        return 1;
    }
    const char *products;
    calc.Fetch("products",products);
    if( products == NULL )
    {
        EOSerror->Log(FUNC, "products not specified\n" );
        return 1;
    }    
    Parameters params;
    params.base = Strdup("EOS");
    params.type = Strdup("PTequilibrium");
    params.name = Strdup(Name());
    char *uname = Cat(units->Type(),"::",units->Name());
    params.units = Strdup(uname);
    char *eos1 = Cat("eos1 = \"",reactants, "\"");
    char *eos2 = Cat("eos2 = \"",products, "\"");
    params.Append(eos1);
    params.Append(eos2);
    params.Append("lambda1 = 1");
    delete [] eos1;
    delete [] eos2;
    delete [] uname;
    HE = new PTequilibrium;
    if( HE->Initialize(params, db) )
    {
        EOSerror->Log(FUNC, "HE->Initialize failed\n");
        delete HE;
        HE = NULL;
        return 1;      
    }
    double tol;
    if( calc.Fetch("tol",tol)==0 )
        HE->tol = tol;
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
int ArrheniusHE::var(const char *name, double V, double e, const double *z,
                         double &value)
{
    return ExtEOS::var(name,V,e,z,value);    
}
