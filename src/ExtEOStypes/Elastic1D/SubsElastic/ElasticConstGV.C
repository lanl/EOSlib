#include "ElasticConstGV.h"
#include "ElasticConstGV_VT.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;


const char *ElasticConstGVlib_vers  = Elastic1D_vers;
const char *ElasticConstGVlib_date  = Elastic1D_date;

extern "C" { // dynamic link for database
  char *EOS_ElasticConstGV_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_ElasticConstGV_Info()
  {
      std::string msg(ElasticConstGVlib_vers);
      msg = msg + " : " + ElasticConstGVlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_ElasticConstGV_Constructor()
  {
    EOS *eos = new ElasticConstGV;
	return static_cast<void *>(eos);
  }
}


ElasticConstGV::ElasticConstGV(const char *ptype) : Elastic1D(ptype)
{
    hydro = NULL;
    shear = NULL;
    GV    = NULL;
    eps_el_ref = 0.0;
}

ElasticConstGV::ElasticConstGV(ElasticConstGV& el) : Elastic1D(el)
{
    if( el.hydro == NULL || el.shear == NULL )
    {
        hydro = NULL;
        shear = NULL;
        GV    = NULL;
        eps_el_ref = 0.0;
        EOSstatus = EOSlib::abort;
        return;
    }
    hydro      = el.hydro->Duplicate();
    GV         = el.GV->Duplicate();
    shear      = static_cast<Shear1D*>(GV);
    eps_el_ref = el.eps_el_ref;
}

ElasticConstGV::ElasticConstGV(ElasticConstGV_VT& el) : Elastic1D(el.Type())
{
    if( el.hydro == NULL || el.shear == NULL )
    {
        hydro = NULL;
        shear = NULL;
        GV    = NULL;
        eps_el_ref = 0.0;
        EOSstatus = EOSlib::abort;
        return;
    }
    Transfer(el);
    hydro      = el.hydro->eosVe();
    GV         = el.GV->Duplicate();
    shear      = static_cast<Shear1D*>(GV);
    eps_el_ref = el.eps_el_ref;
}

ElasticConstGV::~ElasticConstGV()
{
    // Null
}

EOS_VT *ElasticConstGV::VT()
{
    if( hydro == NULL || shear == NULL )
        return NULL;
    return new ElasticConstGV_VT(*this);
}

void ElasticConstGV::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    if( hydro )
    {
        out << "\t       EOS = \"" << hydro->Type()
            << "::" << hydro->Name() << "\"\n";
    }
    if( GV )
    {
        out << "\t      Gref = " << GV->GV/hydro->V_ref << "\n";
    }
    out <<     "\teps_el_ref = " << eps_el_ref << "\n";
}

ostream &ElasticConstGV::PrintComponents(ostream &out)
{
    if( hydro )
        hydro->PrintAll(out);
    return out;
}

int  ElasticConstGV::ConvertParams(Convert &convert)
{
    if( hydro == NULL || GV == NULL )
        return 1;
    return hydro->ConvertUnits(convert) || GV->ConvertUnits(convert)
           || EOS::ConvertParams(convert);    
}

#define FUNC "ElasticConstGV::InitParams",__FILE__,__LINE__,this
int  ElasticConstGV::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    deleteEOS(hydro);
    deleteShear1D(shear);  GV = NULL;
    eps_el_ref = 0.0;
    EOSstatus = EOSlib::abort;
    if( db == NULL )
    {
        EOSerror->Log(FUNC, "initialization requires database\n" );
        return 1;
    }
    char *hydro_name = NULL;
    calc.Variable( "EOS", &hydro_name );
    double Gref;
    calc.Variable( "Gref", &Gref );
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

    Parameters param;
    param.base  = Strdup("Shear1D");
    param.type  = Strdup("ConstGV");
    param.name  = Strdup(p.name);
    param.units = Strdup(p.units);
    param.Append("GV", Gref*hydro->V_ref);
    Units *u = DupUnits();
    GV = new ConstGV;
    if( GV->Initialize(param,NULL,u) )
    {
        EOSerror->Log(FUNC, "GV->Init failed\n");
        return 1;
    }
    deleteUnits(u);
    shear = GV;
    
    if( isnan(V_ref) )
        V_ref = hydro->V_ref;
    if( isnan(e_ref) )
    {
        double T_ref = hydro->T(hydro->V_ref,hydro->e_ref);
        e_ref = hydro->e_ref + GV->e(V_ref,T_ref,eps_el_ref);
    }
	return 0;    
}
