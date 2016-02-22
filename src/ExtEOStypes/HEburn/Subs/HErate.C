#include "HEburn.h"

int HErate::init = 0;

void HErate::Init()
{
    if( init )
        return;
    init = 1;
    Initbase();
}
//
//
HErate::HErate(const char *rtype) : EOSbase("HErate",rtype)
{
    if( !init )
        Init();
    HE = NULL;
}

HErate::~HErate()
{
    if( HE )
    {
        EOS *eos = static_cast<EOS*>(HE);
        deleteEOS(eos);
        HE = NULL;
    }
}

#define FUNC "HErate::InitBase",__FILE__,__LINE__,this
int HErate::InitBase(Parameters &p, Calc &calc, DataBase *db)
{
    EOSstatus = EOSlib::abort;
    HE = NULL;
    if( db == NULL )
    {
        EOSerror->Log(FUNC, "initialization requires database\n" );
        return 1;
    }
    PreInit(calc);          // derived class
	ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC, "parse failed for %s\n",line );
        return 1;
    }
    EOSstatus = EOSlib::good;
	if( PostInit(calc,db) ) // derived class
    {
        EOSstatus = EOSlib::abort;
        return 1;
    }
    return 0;    
}

int HErate::ConvertParams(Convert &convert)
{
    if( HE == NULL )
        return 1;
    return HE->ConvertUnits(convert);
}

#undef FUNC
#define FUNC "HErate::InitBurn",__FILE__,__LINE__,this
int HErate::InitBurn(PTequilibrium *eos)
{
    if( eos != HE )
    {
        if( HE )
        {
            EOS *Eos = static_cast<EOS*>(HE);
            deleteEOS(Eos);
            HE = NULL;
        }
        if( eos )
        {
            HE = new PTequilibrium(*eos);
            /*************/       
            if( units && HE->ConvertUnits(*units) )
            {
                EOSerror->Log(FUNC, "HE->ConvertUnits(%s::%s) failed\n",
                     units->Type(),units->Name() );
                return 1;
            }
            /*************/        
        }
    }
    if( HE == NULL )
    {
        EOSstatus = EOSlib::bad;
        return 1;
    }
    return 0;
}

int HErate::PostInit(Calc &, DataBase *)
{
    return 0;
}
//
//
//
int HErate::set_lambda(double lambda)
{
    if( HE == NULL )
        return 1;
    if( HE->lambda2 != lambda )
    {
        HE->lambda1 = 1.-lambda;
        HE->lambda2 = lambda;
        HE->Vav = -1.;
    }
    return 0;
}

double HErate::P(double V, double e, double lambda)
{
    if( set_lambda(lambda) )
        return EOS::NaN;
    return HE->P(V,e);
}
double HErate::T(double V, double e, double lambda)
{
    if( set_lambda(lambda) )
        return EOS::NaN;
    return HE->T(V,e);
}
double HErate::c(double V, double e, double lambda)
{
    if( set_lambda(lambda) )
        return EOS::NaN;
    return HE->c(V,e);
}
