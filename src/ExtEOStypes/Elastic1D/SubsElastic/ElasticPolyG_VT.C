#include "ElasticPolyG.h"
#include "ElasticPolyG_VT.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;


const char *ElasticPolyG_VTlib_vers  = Elastic1D_VT_vers;
const char *ElasticPolyG_VTlib_date  = Elastic1D_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_ElasticPolyG_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());        
      }
      return NULL;
  }
  
  char *EOS_VT_ElasticPolyG_Info()
  {
      std::string msg(ElasticPolyG_VTlib_vers);
      msg = msg + " : " + ElasticPolyG_VTlib_date
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_VT_ElasticPolyG_Constructor()
  {
    EOS_VT *eos = new ElasticPolyG_VT;
	return static_cast<void *>(eos);
  }
}


ElasticPolyG_VT::ElasticPolyG_VT(const char *ptype) : Elastic1D_VT(ptype)
{
    hydro = NULL;
    shear = NULL;
    PG    = NULL;
    eps_el_ref = 0.0;
}

ElasticPolyG_VT::ElasticPolyG_VT(ElasticPolyG_VT& el) : Elastic1D_VT(el)
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

ElasticPolyG_VT::ElasticPolyG_VT(ElasticPolyG& el) : Elastic1D_VT(el.Type())
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
    hydro      = el.hydro->eosVT();
    PG         = el.PG->Duplicate();
    shear      = static_cast<Shear1D*>(PG);
    eps_el_ref = el.eps_el_ref;
}

ElasticPolyG_VT::~ElasticPolyG_VT()
{
    // Null
}

EOS *ElasticPolyG_VT::Ve()
{
    if( hydro == NULL || shear == NULL )
        return NULL;
    return new ElasticPolyG(*this);
}

void ElasticPolyG_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
    if( hydro )
    {
        out << "\t  EOS = \"" << hydro->Type()
            << "::" << hydro->Name() << "\"\n";
    }
    if( PG )
    {
        out << "\tPolyG = " << PG->Name() << "\n";
    }
    out << "\teps_el_ref = " << eps_el_ref << "\n";
}

ostream &ElasticPolyG_VT::PrintComponents(ostream &out)
{
    if( hydro )
        hydro->PrintAll(out);
    if( PG )
        PG->PrintAll(out);
    return out;
}

int  ElasticPolyG_VT::ConvertParams(Convert &convert)
{
    if( hydro == NULL || PG == NULL )
        return 1;
    return hydro->ConvertUnits(convert) || PG->ConvertUnits(convert)
           || EOS_VT::ConvertParams(convert);    
}

#define FUNC "ElasticPolyG_VT::InitParams",__FILE__,__LINE__,this
int  ElasticPolyG_VT::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    deleteEOS_VT(hydro);
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


    if( isnan(V_ref) )
        V_ref = hydro->V_ref;
    if( isnan(T_ref) )
        T_ref = hydro->T_ref;
	return 0;    
}
