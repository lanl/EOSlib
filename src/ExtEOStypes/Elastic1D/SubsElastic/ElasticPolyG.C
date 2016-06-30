#include "ElasticPolyG.h"
#include "ElasticPolyG_VT.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;


const char *ElasticPolyGlib_vers  = Elastic1D_vers;
const char *ElasticPolyGlib_date  = Elastic1D_date;

extern "C" { // dynamic link for database
  char *EOS_ElasticPolyG_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_ElasticPolyG_Info()
  {
      std::string msg(ElasticPolyGlib_vers);
      msg = msg + " : " + ElasticPolyGlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_ElasticPolyG_Constructor()
  {
    EOS *eos = new ElasticPolyG;
	return static_cast<void *>(eos);
  }
}


ElasticPolyG::ElasticPolyG(const char *ptype) : Elastic1D(ptype)
{
    hydro = NULL;
    shear = NULL;
    PG    = NULL;
    eps_el_ref = 0.0;
}

ElasticPolyG::ElasticPolyG(ElasticPolyG& el) : Elastic1D(el)
{
    if( el.hydro == NULL || el.shear == NULL )
    {
        hydro = NULL;
        shear = NULL;
        PG    = NULL;
        eps_el_ref = 0.0;
        EOSstatus = EOSlib::abort;
        return;
    }
    hydro      = el.hydro->Duplicate();
    PG         = el.PG->Duplicate();
    shear      = static_cast<Shear1D*>(PG);
    eps_el_ref = el.eps_el_ref;
}

ElasticPolyG::ElasticPolyG(ElasticPolyG_VT& el) : Elastic1D(el.Type())
{
    if( el.hydro == NULL || el.shear == NULL )
    {
        hydro = NULL;
        shear = NULL;
        PG    = NULL;
        eps_el_ref = 0.0;
        EOSstatus = EOSlib::abort;
        return;
    }
    Transfer(el);
    hydro      = el.hydro->eosVe();
    PG         = el.PG->Duplicate();
    shear      = static_cast<Shear1D*>(PG);
    eps_el_ref = el.eps_el_ref;
}

ElasticPolyG::~ElasticPolyG()
{
    // Null
}

EOS_VT *ElasticPolyG::VT()
{
    if( hydro == NULL || shear == NULL )
        return NULL;
    return new ElasticPolyG_VT(*this);
}

void ElasticPolyG::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    if( hydro )
    {
        out << "\t       EOS = \"" << hydro->Type()
            << "::" << hydro->Name() << "\"\n";
    }
    if( PG )
    {
        out << "\t     PolyG = " << PG->Name() << "\n";
    }
    out <<     "\teps_el_ref = " << eps_el_ref << "\n";
}

ostream &ElasticPolyG::PrintComponents(ostream &out)
{
    if( hydro )
        hydro->PrintAll(out);
    if( PG )
        PG->PrintAll(out);
    return out;
}

int  ElasticPolyG::ConvertParams(Convert &convert)
{
    if( hydro == NULL || PG == NULL )
        return 1;
    return hydro->ConvertUnits(convert) || PG->ConvertUnits(convert)
           || EOS::ConvertParams(convert);    
}

#define FUNC "ElasticPolyG::InitParams",__FILE__,__LINE__,this
int  ElasticPolyG::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    deleteEOS(hydro);
    deleteShear1D(shear);  PG = NULL;
    eps_el_ref = 0.0;
    EOSstatus = EOSlib::abort;
    if( db == NULL )
    {
        EOSerror->Log(FUNC, "initialization requires database\n" );
        return 1;
    }
    char *hydro_name = NULL;
    char *PG_name    = NULL;
    calc.Variable("EOS",        &hydro_name );
    calc.Variable("PolyG",      &PG_name );
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
    hydro = FetchEOS(hydro_name,*db);
    if( hydro == NULL )
    {
        EOSerror->Log(FUNC, "FetchEOS failed for %s\n", hydro_name );
        return 1;
    }
    if( units && hydro->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "elastic->ConvertUnits(%s) failed\n", p.units );
        return 1;
    }
    if( PG_name == NULL )
    {
        EOSerror->Log(FUNC, "PolyG not specified\n" );
        return 1;
    }
    shear = FetchShear1D("PolyG",PG_name,*db);
    if( shear == NULL )
    {
        EOSerror->Log(FUNC, "FetchShear1D failed for %s\n", PG_name );
        return 1;
    }
    if( units && shear->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "shear->ConvertUnits(%s) failed\n", p.units );
        return 1;
    }
    PG = dynamic_cast<PolyG*>(shear);
    
    if( std::isnan(V_ref) )
        V_ref = hydro->V_ref;
    if( std::isnan(e_ref) )
    {
        double T_ref = hydro->T(hydro->V_ref,hydro->e_ref);
        e_ref = hydro->e_ref + PG->e(V_ref,T_ref,eps_el_ref);
    }
	return 0;    
}
