#include "Elastic.h"

extern "C" { // DataBase functions
  char *EOS_ElasticEOS_Init()
  {
      return NULL;
  }
  char *EOS_ElasticEOS_Info()
  {
      char *msg = Strdup("ElasticEOS derived from Elastic_VT");
      return msg;
  }
  void *EOS_ElasticEOS_Constructor()
  {
      EOS *eos = new ElasticEOS;
      return static_cast<void*>(eos);
  }
}


ElasticEOS::ElasticEOS() : ExtEOS("ElasticEOS")
{
    elastic = NULL;
      hydro = NULL;
}

ElasticEOS::ElasticEOS(Elastic_VT &VT) : ExtEOS("ElasticEOS")
{
    n = 0; ztmp = NULL;
    if( (elastic=VT.Duplicate()) == NULL )
    {
        hydro = NULL;
        EOSstatus = EOSlib::abort;
        return;
    }
    z_ref = elastic->z_ref()->Copy();
    n = z_ref->N();
    Replace(NULL);
    if( (hydro=elastic->Hydro_Ve()) == NULL )
    {
        EOSstatus = EOSlib::abort;
        return;
    }
    units = VT.DupUnits();
    V_ref = VT.V_ref;
    e_ref = VT.e(V_ref, VT.T_ref);
    Rename(VT.Name());
    EOSstatus = EOSlib::good;
}

ElasticEOS::ElasticEOS(ElasticEOS &eos) : ExtEOS(eos)
{
    if( eos.elastic )
    {
        elastic = eos.elastic->Duplicate();
          hydro = elastic->Hydro_Ve();
    }
    else
    {
        elastic = NULL;
          hydro = NULL;
    } 
}

ElasticEOS::ElasticEOS(ElasticEOS &eos, const double *z) : ExtEOS(eos)
{
    if( eos.elastic )
    {
        elastic = static_cast<Elastic_VT*>(eos.elastic->Duplicate(z));
        z_ref = elastic->z_ref()->Copy();  
        n = z_ref->N();
        Replace(NULL);
        hydro = elastic->Hydro_Ve();
    }
    else
    {
        elastic = NULL;
          hydro = NULL;
    } 
}

ElasticEOS::~ElasticEOS()
{
    deleteElastic_VT(elastic);
    deleteEOS(hydro);
}

ExtEOS *ElasticEOS::Copy()
{
    return NULL;    // replaced by derived class
}

ExtEOS *ElasticEOS::Copy(const double *z)
{
    if( elastic == NULL )
        return NULL;
    return new ElasticEOS(*this, z);
}



void ElasticEOS::PrintParams(ostream &out)
{
    ExtEOS::PrintParams(out);
    if( elastic )
    {
        out << "\tElastic_VT = \"" << elastic->Type()
            << "::" << elastic->Name() << "\"\n";
    }
}
int ElasticEOS::ConvertParams(Convert &convert)
{
    if( EOSstatus != EOSlib::good )
        return 1;
    return elastic->ConvertUnits(convert) || hydro->ConvertUnits(convert)
           || ExtEOS::ConvertParams(convert);
}

ostream &ElasticEOS::PrintComponents(ostream &out)
{
    if( elastic )
        elastic->PrintAll(out);
    return out;
}

int ElasticEOS::PreInit(Calc &calc)
{
    deleteElastic_VT(elastic);
    deleteEOS(hydro);
    char *elastic_name = NULL;
    calc.Variable( "Elastic_VT", &elastic_name );
    return 0;
}


#define FUNC "ElasticEOS::PostInit",__FILE__,__LINE__,this
int ElasticEOS::PostInit(Calc &calc, DataBase *db)
{
    const char *elastic_name;
    calc.Fetch("Elastic_VT", elastic_name);
    if( elastic_name == NULL )
    {
        EOSerror->Log(FUNC, "Elastic_VT not specified\n" );
        return 1;
    }
    EOS_VT *eos = FetchEOS_VT(elastic_name,*db);
    if( eos == NULL )
    {
        EOSerror->Log(FUNC, "FetchEOS_VT failed for %s\n", elastic_name );
        return 1;
    }
    elastic = dynamic_cast<Elastic_VT*>(eos);
    if( elastic == NULL )
    {
        EOSerror->Log(FUNC, "EOS::%s, not of type Elastic_VT\n", elastic_name );
        return 1;
    }    
    if( units && elastic->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "elastic->ConvertUnits(%s::%s) failed\n",
                      units->Type(), units->Name() );
        return 1;
    }
    z_ref = elastic->z_ref()->Copy();    
    hydro = elastic->Hydro_Ve();
    if( hydro == NULL )
    {
        EOSerror->Log(FUNC, "elastic->Hydro_Ve failed\n" );
        return 1;
    }
    if( isnan(V_ref) )
        V_ref = elastic->V_ref;
    if( isnan(e_ref) )
        e_ref = elastic->e(V_ref,elastic->T_ref);
	return 0;
}
