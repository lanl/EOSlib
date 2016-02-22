#include "ElasticPlastic.h"

const char *ElasticPlastic_lib_vers = ElasticPlastic_vers;
const char *ElasticPlastic_lib_date = ElasticPlastic_date;

ElasticPlastic::ElasticPlastic(const char *ptype) : ExtEOS(ptype)
{
    elastic = NULL;
    hydro   = NULL;
    z_el    = NULL;
 //
    z_ref = new PlasticStrain;
}

ElasticPlastic::ElasticPlastic(const ElasticPlastic &EP) : ExtEOS(EP)
{
    if( EP.elastic == NULL )
    {
        elastic = NULL;
        EOSstatus = EOSlib::abort;
        return;        
    }
    elastic = EP.elastic->Duplicate();
    hydro = elastic->Hydro_Ve();
    z_el = new double[n];
}

ElasticPlastic::~ElasticPlastic()
{
    deleteElastic_VT(elastic);
    deleteEOS(hydro);
    delete [] z_el;
}

ostream &ElasticPlastic::PrintComponents(ostream &out)
{
    if( elastic )
        elastic->PrintAll(out);
    return out;
}

void ElasticPlastic::PrintParams(ostream &out)
{
    ExtEOS::PrintParams(out);
    if( elastic )
    {
        out << "\tElastic = \"" << elastic->Type()
            << "::" << elastic->Name() << "\"\n";
    }
}

int ElasticPlastic::ConvertParams(Convert &convert)
{
    if( EOSstatus != EOSlib::good )
        return 1;
    return elastic->ConvertUnits(convert) || hydro->ConvertUnits(convert)
           || ExtEOS::ConvertParams(convert);
}

int ElasticPlastic::PreInit(Calc &calc)
{
    deleteElastic_VT(elastic);
    deleteEOS(hydro);
    delete [] z_el;
    z_el=NULL;
    char *elastic_name = NULL;
    calc.Variable( "Elastic", elastic_name );
    return 0;
}

    
#define FUNC "ElasticPlastic::PostInit",__FILE__,__LINE__,this
int ElasticPlastic::PostInit(Calc &calc, DataBase *db)
{
    const char *elastic_name;
    calc.Fetch("Elastic", elastic_name );
    if( elastic_name == NULL )
    {
        EOSerror->Log(FUNC, "Elastic not specified\n" );
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
    hydro = elastic->Hydro_Ve();
    if( hydro == NULL )
    {
        EOSerror->Log(FUNC, "elastic->Hydro_Ve failed\n" );
        return 1;
    }
    z_el = new double[elastic->ExtEOS_VT::N()];
    if( isnan(V_ref) )
        V_ref = elastic->V_ref;
    if( isnan(e_ref) )
    {
        z_el[0] = elastic->eps_el(V_ref) - (*z_ref)[0];
        e_ref = elastic->e(V_ref,elastic->T_ref,z_el);
    }
	return 0;
}

int ElasticPlastic::var(const char *name, double V, double e, const double *z,
                            double &value)
{
    if( !strcmp(name,"yield_function") )
    {
        value = yield_func(V,e,z);
        return 0;
    }
    if( !strcmp(name,"yield_strength") )
    {
        value = yield_surf(V,e,z);
        return 0;
    }
    if( !strcmp(name,"eps_el") )
    {
        value = eps_el(V,e,z);
        return 0;
    }
    if( !strcmp(name,"Phydro") )
    {
        value = Phydro(V,e,z);
        return 0;
    }
    if( !strcmp(name,"Pshear") )
    {
        value = Pshear(V,e,z);
        return 0;
    }
    if( !strcmp(name,"Pdev") )
    {
        value = -shear(V,e,z);
        return 0;
    }
    return ExtEOS::var(name,V,e,z,value);    
}
