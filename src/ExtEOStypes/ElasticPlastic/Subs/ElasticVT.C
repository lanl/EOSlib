#include "Elastic.h"

const char *Elastic_VTlib_vers  = Elastic_VT_vers;
const char *Elastic_VTlib_date  = Elastic_VT_date;

Elastic_VT::Elastic_VT(const char *ptype) : ExtEOS_VT(ptype)
{
    hydro = NULL;
    ExtEOS_VT::n = ElasticStrain::N();
    ztmp = new double[ExtEOS_VT::n];
}

Elastic_VT::Elastic_VT(const Elastic_VT &el)
            : ExtEOS_VT(el), ElasticStrain(el)
{
    if( el.hydro == NULL )
    {
        hydro = NULL;
        EOSstatus = EOSlib::abort;
        return;        
    }
    hydro = el.hydro->Duplicate();
}

Elastic_VT::~Elastic_VT()
{
    deleteEOS_VT(hydro);
}

ExtEOS_VT *Elastic_VT::Duplicate(const double *z)
{
    return NULL;
}

ostream &Elastic_VT::PrintComponents(ostream &out)
{
    if( hydro )
        hydro->PrintAll(out);
    return out;
}

void Elastic_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
    if( hydro )
    {
        out << "\tEOS_VT = \"" << hydro->Type()
            << "::" << hydro->Name() << "\"\n";
    }
    ElasticStrain::PrintParams(out);
}

int Elastic_VT::ConvertParams(Convert &convert)
{
    if( hydro == NULL )
        return 1;
    return hydro->ConvertUnits(convert)
           || ElasticStrain::ConvertParams(convert)
           || ExtEOS_VT::ConvertParams(convert);
}

int Elastic_VT::PreInit(Calc &)
{
    return 1;
}

int Elastic_VT::PostInit()
{
    return 0;
}

#define FUNC "Elastic_VT::InitParams",__FILE__,__LINE__,this
int Elastic_VT::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    deleteEOS_VT(hydro);
    EOSstatus = EOSlib::abort;
    if( db == NULL )
    {
        EOSerror->Log(FUNC, "initialization requires database\n" );
        return 1;
    }
    char *hydro_name = NULL;
    calc.Variable( "EOS_VT", &hydro_name );
    if( ElasticStrain::InitParams(calc) )
    {
        EOSerror->Log(FUNC, "IDOF::InitParams failed\n" );
        return 1;        
    }
    if( PreInit(calc) )
    {
        EOSerror->Log(FUNC, "PreInit failed\n" );
        return 1;        
    }

	ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( hydro_name == NULL )
    {
        EOSerror->Log(FUNC,"EOS_VT not specified\n");
        return 1;
    }
    hydro = FetchEOS_VT(hydro_name,*db);
    if( hydro == NULL )
    {
        EOSerror->Log(FUNC,"FetchEOS_VT failed for %s\n", hydro_name);
        return 1;
    }
    if( units && hydro->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC,"hydro->ConvertUnits(%s) failed\n", p.units );
        return 1;
    }

    if( isnan(V_ref) )
        V_ref = hydro->V_ref;
    if( isnan(T_ref) )
        T_ref = hydro->T_ref;

    EOSstatus = EOSlib::good;
	return PostInit();
}
