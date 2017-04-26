#include "HErate2.h"

int HErate2::init = 0;

void HErate2::Init()
{
    if( init )
        return;
    init = 1;
    Initbase();
}
//
//
HErate2::HErate2(const char *rtype) : EOSbase("HErate2",rtype)
{
    if( !init )
        Init();
    HE = NULL;
}

HErate2::~HErate2()
{
    if( HE )
    {
        EOS *eos = static_cast<EOS*>(HE);
        deleteEOS(eos);
        HE = NULL;
    }
}

#define FUNC "HErate2::InitBase",__FILE__,__LINE__,this
int HErate2::InitBase(Parameters &p, Calc &calc, DataBase *db)
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

int HErate2::ConvertParams(Convert &convert)
{
    if( HE == NULL )
        return 1;
    return HE->ConvertUnits(convert);
}

#undef FUNC
#define FUNC "HErate2::InitBurn",__FILE__,__LINE__,this
int HErate2::InitBurn(PTequilibrium *eos)
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

int HErate2::PostInit(Calc &, DataBase *)
{
    return 0;
}
//
//
//
int HErate2::set_eos(double lambda, double lambda2)
{
    if( HE == NULL )
        return 1;
    double de = e2(lambda2);
    HE->set_lambda2(lambda, -de);
    return 0;
}
int HErate2::set_eos(const double *z)
{
    if( z==NULL || HE == NULL )
        return 1;
    double lambda, lambda2;
    if( get_lambda(z, lambda, lambda2) )
        return 1;
    double de = e2(lambda2);
    HE->set_lambda2(lambda, -de);
    return 0;
}

int HErate2::dPdlambda(double V, double e, double lambda,double lambda2,
                       double &dP1, double &dP2)
{
    if( set_eos(lambda, lambda2) )
        return 1;

    double dP, dT;
    if( HE->ddlambda(V,e, dP,dT) ) // d/d(HE->lambda1) = -d/dlambda
        return 1;
    double GammaV = HE->Gamma(V,e)/V;
   
    double de1, de2;
    (void) q(lambda,lambda2, de1,de2);

    // Note, dP includes term (Gamma/V)*de1
    dP1 = -dP;                  // dP/dlambda  at const V,e
    dP2 = -GammaV*de2;          // dP/dlambda2 at const V,e

    return 0;
}

int HErate2::dTdlambda(double V, double e, double lambda,double lambda2,
                       double &dT1, double &dT2)
{
    if( set_eos(lambda, lambda2) )
        return 1;

    double dP, dT;
    if( HE->ddlambda(V,e, dP,dT) ) // d/d(HE->lambda1) = -d/dlambda
        return 1;
    double Cv = HE->CV(V,e);

    double de1, de2;
    (void) q(lambda,lambda2, de1,de2);

    dT1 = -(dT + de1/Cv);
    dT2 = - de2/Cv;

    return 0;
}


double HErate2::P(double V, double e, double lambda, double lambda2)
{
    if( set_eos(lambda, lambda2) )
        return EOS::NaN;
    return HE->P(V,e);
}
double HErate2::T(double V, double e, double lambda, double lambda2)
{
    if( set_eos(lambda, lambda2) )
        return EOS::NaN;
    return HE->T(V,e);
}
double HErate2::c(double V, double e, double lambda, double lambda2)
{
    if( set_eos(lambda, lambda2) )
        return EOS::NaN;
    return HE->c(V,e);
}

int HErate2::var(const char *name, double V, double e, const double *z,
                       double &value)
{
    return 1;
}

int HErate2::set_z(const char *var, double value, double *z)
{
    return 1;
}

int HErate2::get_z(double *z, const char *var, double &value)
{
    return 1;
}

