#include "ElasticConstGV.h"
#include "ElasticConstGV_VT.h"

#include <iostream>
#include <iomanip>
using namespace std;
#include <string>

const char *ElasticConstGV_VTlib_vers  = Elastic1D_VT_vers;
const char *ElasticConstGV_VTlib_date  = Elastic1D_VT_date;

extern "C" { // dynamic link for database
  char *EOS_VT_ElasticConstGV_Init()
  {
      if( EOS_VTlib_mismatch(EOS_VT_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOS_VTlib_vers + "), derived(" + EOS_VT_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VT_ElasticConstGV_Info()
  {
      std::string msg(ElasticConstGV_VTlib_vers);
      msg = msg + " : " + ElasticConstGV_VTlib_date
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_VT_ElasticConstGV_Constructor()
  {
    EOS_VT *eos = new ElasticConstGV_VT;
	return static_cast<void *>(eos);
  }
}


ElasticConstGV_VT::ElasticConstGV_VT(const char *ptype) : Elastic1D_VT(ptype)
{
    hydro = NULL;
    shear = NULL;
    GV    = NULL;
    eps_el_ref = 0.0;
}

ElasticConstGV_VT::ElasticConstGV_VT(ElasticConstGV_VT& el) : Elastic1D_VT(el)
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

ElasticConstGV_VT::ElasticConstGV_VT(ElasticConstGV& el) : Elastic1D_VT(el.Type())
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
    hydro      = el.hydro->eosVT();
    GV         = el.GV->Duplicate();
    shear      = static_cast<Shear1D*>(GV);
    eps_el_ref = el.eps_el_ref;
}

ElasticConstGV_VT::~ElasticConstGV_VT()
{
    // Null
}

EOS *ElasticConstGV_VT::Ve()
{
    if( hydro == NULL || shear == NULL )
        return NULL;
    return new ElasticConstGV(*this);
}

void ElasticConstGV_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
    if( hydro )
    {
        out << "\tEOS = \"" << hydro->Type()
            << "::" << hydro->Name() << "\"\n";
    }
    if( GV )
    {
        out << "\t      Gref = " << GV->GV/hydro->V_ref << "\n";
    }
    out << "\teps_el_ref = " << eps_el_ref << "\n";
}

ostream &ElasticConstGV_VT::PrintComponents(ostream &out)
{
    if( hydro )
        hydro->PrintAll(out);
    return out;
}

int  ElasticConstGV_VT::ConvertParams(Convert &convert)
{
    if( hydro == NULL || GV == NULL )
        return 1;
    return hydro->ConvertUnits(convert) || GV->ConvertUnits(convert)
           || EOS_VT::ConvertParams(convert);    
}

#define FUNC "ElasticConstGV_VT::InitParams",__FILE__,__LINE__,this
int  ElasticConstGV_VT::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    deleteEOS_VT(hydro);
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
    if( isnan(T_ref) )
        T_ref = hydro->T_ref;
	return 0;    
}
