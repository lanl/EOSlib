#include "Elastic1D.h"
#include "Elastic1D_VT.h"

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;


const char *Elastic1Dlib_vers  = Elastic1D_vers;
const char *Elastic1Dlib_date  = Elastic1D_date;

extern "C" { // dynamic link for database
  char *EOS_Elastic1D_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_Elastic1D_Info()
  {
      std::string msg(Elastic1Dlib_vers);
      msg = msg + " : " + Elastic1Dlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_Elastic1D_Constructor()
  {
    EOS *eos = new Elastic1D;
	return static_cast<void *>(eos);
  }
}


Elastic1D::Elastic1D(const char *ptype) : EOS(ptype)
{
    hydro = NULL;
    shear = NULL;
    eps_el_ref = 0.0;
}

Elastic1D::Elastic1D(Elastic1D& el) : EOS(el)
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

Elastic1D::Elastic1D(Elastic1D_VT& el) : EOS(el.Type())
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
    hydro      = el.hydro->eosVe();
    shear      = el.shear->Duplicate();
    eps_el_ref = el.eps_el_ref;
}

Elastic1D::~Elastic1D()
{
    deleteEOS(hydro);
    deleteShear1D(shear);
}

EOS_VT *Elastic1D::VT()
{
    if( hydro == NULL || shear == NULL )
        return NULL;
    return new Elastic1D_VT(*this);
}

void Elastic1D::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    if( hydro )
    {
        out << "\t       EOS = \"" << hydro->Type()
            << "::" << hydro->Name() << "\"\n";
    }
    if( shear )
    {
        out << "\t   Shear1D = \"" << shear->Type()
            << "::" << shear->Name() << "\"\n";
    }
    out <<     "\teps_el_ref = " << eps_el_ref << "\n";
}

ostream &Elastic1D::PrintComponents(ostream &out)
{
    if( hydro )
        hydro->PrintAll(out);
    if( shear )
        shear->PrintAll(out);
    return out;
}

int  Elastic1D::ConvertParams(Convert &convert)
{
    if( hydro == NULL || shear == NULL )
        return 1;
    return hydro->ConvertUnits(convert) || shear->ConvertUnits(convert)
           || EOS::ConvertParams(convert);    
}

#define FUNC "Elastic1D::InitParams",__FILE__,__LINE__,this
int  Elastic1D::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    deleteEOS(hydro);
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

    if( std::isnan(V_ref) )
        V_ref = hydro->V_ref;
    if( std::isnan(e_ref) )
    {
        double T_ref = hydro->T(hydro->V_ref,hydro->e_ref);
        e_ref = hydro->e_ref + shear->e(V_ref,T_ref,eps_el_ref);
    }
	return 0;    
}
