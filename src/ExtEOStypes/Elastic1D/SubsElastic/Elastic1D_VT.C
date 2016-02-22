#include "Elastic1D.h"
#include "Elastic1D_VT.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;


const char *Elastic1D_VTlib_vers  = Elastic1D_VT_vers;
const char *Elastic1D_VTlib_date  = Elastic1D_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_Elastic1D_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  
  char *EOS_VT_Elastic1D_Info()
  {
      std::string msg(Elastic1D_VTlib_vers);
      msg = msg + " : " + Elastic1D_VTlib_date
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_VT_Elastic1D_Constructor()
  {
    EOS_VT *eos = new Elastic1D_VT;
	return static_cast<void *>(eos);
  }
}


Elastic1D_VT::Elastic1D_VT(const char *ptype) : EOS_VT(ptype)
{
    hydro = NULL;
    shear = NULL;
    eps_el_ref = 0.0;
}

Elastic1D_VT::Elastic1D_VT(Elastic1D_VT& el) : EOS_VT(el)
{
    if( el.hydro == NULL || el.shear == NULL )
    {
        hydro = NULL;
        shear = NULL;
        eps_el_ref = 0.0;
        EOSstatus = EOSlib::abort;
        return;
    }
    hydro      = el.hydro->Duplicate();
    shear      = el.shear->Duplicate();
    eps_el_ref = el.eps_el_ref;
}

Elastic1D_VT::Elastic1D_VT(Elastic1D& el) : EOS_VT(el.Type())
{
    if( el.hydro == NULL || el.shear == NULL )
    {
        hydro = NULL;
        shear = NULL;
        eps_el_ref = 0.0;
        EOSstatus = EOSlib::abort;
        return;
    }
    Transfer(el);
    hydro      = el.hydro->eosVT();
    shear      = el.shear->Duplicate();
    eps_el_ref = el.eps_el_ref;
}

Elastic1D_VT::~Elastic1D_VT()
{
    deleteEOS_VT(hydro);
    deleteShear1D(shear);
}

EOS *Elastic1D_VT::Ve()
{
    if( hydro == NULL || shear == NULL )
        return NULL;
    return new Elastic1D(*this);
}

void Elastic1D_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
    if( hydro )
    {
        out << "\tEOS = \"" << hydro->Type()
            << "::" << hydro->Name() << "\"\n";
    }
    if( shear )
    {
        out << "\tShear1D = \"" << shear->Type()
            << "::" << shear->Name() << "\"\n";
    }
    out << "\teps_el_ref = " << eps_el_ref << "\n";
}

ostream &Elastic1D_VT::PrintComponents(ostream &out)
{
    if( hydro )
        hydro->PrintAll(out);
    if( shear )
        shear->PrintAll(out);
    return out;
}

int  Elastic1D_VT::ConvertParams(Convert &convert)
{
    if( hydro == NULL || shear == NULL )
        return 1;
    return hydro->ConvertUnits(convert) || shear->ConvertUnits(convert)
           || EOS_VT::ConvertParams(convert);    
}

#define FUNC "Elastic1D_VT::InitParams",__FILE__,__LINE__,this
int  Elastic1D_VT::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    deleteEOS_VT(hydro);
    deleteShear1D(shear);
    eps_el_ref = 0.0;
    EOSstatus = EOSlib::abort;
    if( db == NULL )
    {
        EOSerror->Log(FUNC, "initialization requires database\n" );
        return 1;
    }
    char *hydro_name = NULL;
    calc.Variable( "EOS", &hydro_name );
    char *shear_name = NULL;
    calc.Variable( "Shear1D", &shear_name );
    calc.Variable("eps_el_ref", &eps_el_ref);

	ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC, "parse failed for %s\n",line );
        return 1;
    }
    if( hydro_name == NULL )
    {
        EOSerror->Log(FUNC, "EOS not specified\n" );
        return 1;
    }
    hydro = FetchEOS_VT(hydro_name,*db);
    if( hydro == NULL )
    {
        EOSerror->Log(FUNC, "FetchEOS_VT failed for %s\n", hydro_name );
        return 1;
    }
    if( units && hydro->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "elastic->ConvertUnits(%s) failed\n", p.units );
        return 1;
    }
    if( shear_name == NULL )
    {
        EOSerror->Log(FUNC, "Shear1D not specified\n" );
        return 1;
    }
    shear = FetchShear1D(shear_name,*db);
    if( shear == NULL )
    {
        EOSerror->Log(FUNC, "FetchShear1D failed for %s\n", shear_name );
        return 1;
    }
    if( units && shear->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "shear->ConvertUnits(%s) failed\n", p.units );
        return 1;
    }

    if( isnan(V_ref) )
        V_ref = hydro->V_ref;
    if( isnan(T_ref) )
        T_ref = hydro->T_ref;
	return 0;    
}
