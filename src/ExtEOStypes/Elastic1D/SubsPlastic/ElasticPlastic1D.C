#include "ElasticPlastic1D.h"

#include <iostream>
#include <iomanip>
using namespace std;

const char *ElasticPlastic1D_lib_vers = ElasticPlastic1D_vers;
const char *ElasticPlastic1D_lib_date = ElasticPlastic1D_date;

ElasticPlastic1D::ElasticPlastic1D(const char *ptype) : ExtEOS(ptype)
{
    elastic = NULL;
}

ElasticPlastic1D::ElasticPlastic1D(const ElasticPlastic1D &EP) : ExtEOS(EP)
{
    if( EP.elastic == NULL )
    {
        elastic   = NULL;
        EOSstatus = EOSlib::abort;
        return;        
    }
    elastic = EP.elastic->Duplicate();
}

ElasticPlastic1D::~ElasticPlastic1D()
{
    EOS *eos = static_cast<EOS*>(elastic);
    deleteEOS(eos);
}

ostream &ElasticPlastic1D::PrintComponents(ostream &out)
{
    if( elastic )
        elastic->PrintAll(out);
    return out;
}

void ElasticPlastic1D::PrintParams(ostream &out)
{
    ExtEOS::PrintParams(out);
    if( elastic )
    {
        out << "\tElastic = \"" << elastic->Type()
            << "::" << elastic->Name() << "\"\n";
    }
}

int ElasticPlastic1D::ConvertParams(Convert &convert)
{
    if( elastic == NULL  )
        return 1;
    return elastic->ConvertUnits(convert) || EOS::ConvertParams(convert);
}

int ElasticPlastic1D::PreInit(Calc &calc)
{
    if( z_ref == NULL )
    { // sanity check
        EOSerror->Log("ElasticPlastic1D::PreInit",__FILE__,__LINE__,this,
                      "z_ref is NULL, PROGRAM ERROR !\n");
        return 1;
    }
    EOS *eos = static_cast<EOS*>(elastic);
    deleteEOS(eos);     elastic = NULL;
    char *elastic_name = NULL;
    calc.Variable( "Elastic", elastic_name );
    return 0;
}

#define FUNC "ElasticPlastic1D::PostInit",__FILE__,__LINE__,this
int ElasticPlastic1D::PostInit(Calc &calc, DataBase *db)
{
    const char *elastic_name;
    calc.Fetch("Elastic",elastic_name);
    if( elastic_name == NULL )
    {
        EOSerror->Log(FUNC, "Elastic not specified\n" );
        return 1;
    }
    EOS *eos = FetchEOS(elastic_name,*db);
    if( eos == NULL )
    {
        EOSerror->Log(FUNC, "FetchEOS failed for %s\n", elastic_name );
        return 1;
    }
    elastic = dynamic_cast<Elastic1D*>(eos);
    if( elastic == NULL )
    {
        EOSerror->Log(FUNC, "EOS::%s, not of type Elastic1D\n", elastic_name );
        return 1;
    }    
    if( units && elastic->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "elastic->ConvertUnits(%s::%s) failed\n",
                     units->Type(),units->Name() );
        return 1;
    }
    if( std::isnan(V_ref) )
        V_ref = elastic->V_ref;
    if( std::isnan(e_ref) )
        e_ref = elastic->e_ref;
    return 0;
}

int ElasticPlastic1D::var(const char *name, double V, double e, const double *z,
                              double &value)
{
    if( !strcmp(name,"yield_function") )
    {
        value = yield_function(V,e,z);
        return 0;
    }
    if( !strcmp(name,"yield_strength") )
    {
        value = yield_strength(V,e,z);
        return 0;
    }
    if( !strcmp(name,"eps_el") )
    {
        value = Eps_el(V,e,z);
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
        value = Pdev(V,e,z);
        return 0;
    }
    return ExtEOS::var(name,V,e,z,value);    
}
