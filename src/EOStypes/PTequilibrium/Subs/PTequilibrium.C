#include "PTequilibrium.h"
#include "PTequilibrium_VT.h"
//
// defaults
#define TOL 1e-6
#define LAMBDA_MAX 0.98
#define MAX_ITER  61
#define MAX_ITER1 24
#define TOL_P  max(eos1->P_vac,eos2->P_vac);
#define TOL_T  0.01;
//
PTequilibrium::PTequilibrium() : EOS("PTequilibrium")
{
    eos1    = eos2    = NULL;
    lambda1 = lambda2 = -1.;
    de = 0.;
    P1_ref = T1_ref = 0.0;
    P2_ref = T2_ref = 0.0;
    tol    = TOL;
    tol_P  = 0.0;
    tol_T  = 0.0;
    lambda1_max = LAMBDA_MAX;
    lambda2_max = LAMBDA_MAX;
    max_iter    = MAX_ITER;
    max_iter1   = MAX_ITER1;
    V1  = V2  = 0.0;
    e1  = e2  = 0.0;
    P1  = P2  = 0.0;
    T1  = T2  = 0.0;
    Vav = eav = -1.23456;
    Peq = Teq = 0.0;
}

PTequilibrium::PTequilibrium(const PTequilibrium &eos) : EOS(eos)
{
    lambda1 = eos.lambda1;
    lambda2 = eos.lambda2;
    de      = eos.de;
    P1_ref  = eos.P1_ref;
    T1_ref  = eos.T1_ref;
    P2_ref  = eos.P2_ref;
    T2_ref  = eos.T2_ref;
    tol     = eos.tol;
    tol_P   = eos.tol_P;
    tol_T   = eos.tol_T;
    lambda1_max = eos.lambda1_max;
    lambda2_max = eos.lambda2_max;
    max_iter    = eos.max_iter;
    max_iter1   = eos.max_iter1;
    V1  = eos.V1;
    V2  = eos.V2;
    e1  = eos.e1;
    e2  = eos.e2;
    P1  = eos.P1;
    P2  = eos.P2;
    T1  = eos.T1;
    T2  = eos.T2;
    Vav = eos.Vav;
    eav = eos.eav;
    Peq = eos.Peq;
    Teq = eos.Teq;
    if( eos.eos1 == NULL || eos.eos2 == NULL )
    {
        eos1 = eos2 = NULL;
        Vav  = eav  = -1.23456;
        EOSstatus = EOSlib::abort;
        return;
    }
    eos1 = eos.eos1->Duplicate();
    eos2 = eos.eos2->Duplicate();
    if( (eos1!=NULL)  && (eos2!=NULL) )
        EOSstatus = EOSlib::good;
    else
    {
	    deleteEOS(eos1);
	    deleteEOS(eos2);   
        eos1 = eos2 = NULL;
        Vav  = eav  = -1.23456;
        EOSstatus = EOSlib::abort;
    }
}

PTequilibrium::PTequilibrium(PTequilibrium_VT &eos) : EOS("PTequilibrium")
{
    Transfer(eos);
    lambda1 = eos.lambda1;
    lambda2 = eos.lambda2;
    de      = eos.de;
    P1_ref = T1_ref = 0.0;
    P2_ref = T2_ref = 0.0;
    tol   = eos.tol;
    // ToDo: upgrade PTequilibrium_VT with additional control parameters
    tol_P = 0.;
    tol_T = 0.;
    lambda1_max = LAMBDA_MAX;
    lambda2_max = LAMBDA_MAX;
    max_iter    = MAX_ITER;
    max_iter1   = MAX_ITER1;
    //
    V1  = V2  = 0.0;
    e1  = e2  = 0.0;
    P1  = P2  = 0.0;
    T1  = T2  = 0.0;
    Vav = eav = -1.23456;
    Peq = Teq = 0.0;    
    if( eos.eos1 == NULL || eos.eos2 == NULL )
    {
        eos1 = eos2 = NULL;
        EOSstatus = EOSlib::abort;
        return;
    }
    eos1 = eos.eos1->eosVe();
    eos2 = eos.eos2->eosVe();
    if( eos1==NULL || eos2==NULL )
    {
	    deleteEOS(eos1);
	    deleteEOS(eos2);   
        eos1 = eos2 = NULL;
        EOSstatus = EOSlib::abort;
    }
    V1 = eos1->V_ref;
    e1 = eos1->e_ref;
    V2 = eos2->V_ref;
    e2 = eos2->e_ref;
    P1 = P1_ref = eos1->P(V1,e1);
    T1 = T1_ref = eos1->T(V1,e1);
    P2 = P2_ref = eos2->P(V2,e2);
    T2 = T2_ref = eos2->T(V2,e2);
    tol_P = TOL_P;
    tol_T = TOL_T;
    EOSstatus = EOSlib::good;
}

PTequilibrium::~PTequilibrium()
{
	deleteEOS(eos1);
	deleteEOS(eos2);   
}

EOS_VT *PTequilibrium::VT()
{
    return new PTequilibrium_VT(*this);
}

void PTequilibrium::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    if( eos1 )
        out << "\teos1 = \"" << eos1->Type() << "::" << eos1->Name() << "\"\n";
    if( eos2 )
        out << "\teos2 = \"" << eos2->Type() << "::" << eos2->Name() << "\"\n";
    out << "\tlambda1 = " << lambda1 << "\n";
    out << "\tde      = " << de      << "\n";
    out << "\ttol     = " << tol     << "\n";
    out << "\ttol_P   = " << tol_P   << "\n";
    out << "\ttol_T   = " << tol_T   << "\n";
    out << "\tlambda1_max = " << lambda1_max << "\n";
    out << "\tlambda2_max = " << lambda2_max << "\n";
    out << "\tmax_iter  = "   << max_iter  << "\n";
    out << "\tmax_iter1 = "   << max_iter1 << "\n";
}
ostream &PTequilibrium::PrintComponents(ostream &out)
{
    if( eos1 )
        eos1->PrintAll(out);
    if( eos2 )
        eos2->PrintAll(out);
    return out;
}

int PTequilibrium::ConvertParams(Convert &convert)
{
    if( EOSstatus != EOSlib::good )
        return 1;
    if( EOS::ConvertParams(convert)
        || eos1->ConvertUnits(convert)
        || eos2->ConvertUnits(convert) )
    {
        EOSstatus = EOSlib::bad;
        return -1;
    }
    double s_V, s_e, s_P, s_T;
	if(  !finite(s_V = convert.factor("V"))
	     || !finite(s_e = convert.factor("e"))
	     || !finite(s_P = convert.factor("P"))
	     || !finite(s_T = convert.factor("T")) )
	{
        EOSstatus = EOSlib::bad;
        return -1;	    
	}
    P1_ref *= s_P;
    P2_ref *= s_P;
    P1     *= s_P;
    P2     *= s_P;
    Peq    *= s_P;
    tol_P  *= s_P;
    T1_ref *= s_T;
    T2_ref *= s_T;
    T1     *= s_T;
    T2     *= s_T;
    Teq    *= s_T;
    tol_T  *= s_T;
    V1     *= s_V;
    V2     *= s_V;
    Vav    *= s_V;
    de     *= s_e;
    e1     *= s_e;
    e2     *= s_e;
    eav    *= s_e;   
    return 0;  
}

#define FUNC "PTequilibrium::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int PTequilibrium::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    deleteEOS(eos1);
    deleteEOS(eos2);
    lambda1 = lambda2 = -1.;
    de = 0.0;
    P1_ref = T1_ref = 0.0;
    P2_ref = T2_ref = 0.0;
    tol    = TOL;
    tol_P  = -1.;
    tol_T  = -1.;
    lambda1_max = LAMBDA_MAX;
    lambda2_max = LAMBDA_MAX;
    max_iter    = MAX_ITER;
    max_iter1   = MAX_ITER1;
    V1  = V2  = 0.0;
    e1  = e2  = 0.0;
    P1  = P2  = 0.0;
    T1  = T2  = 0.0;
    Vav = eav = -1.23456;
    Peq = Teq = 0.0;    
    char *eos1_name = NULL;
    calc.Variable( "eos1", &eos1_name );
    char *eos2_name = NULL;
    calc.Variable( "eos2", &eos2_name );
    calcvar(lambda1);
    calcvar(de);
    calcvar(tol);
    calcvar(tol_P);
    calcvar(tol_T);
    calcvar(lambda1_max);
    calcvar(lambda2_max);
    calcvar(max_iter);
    calcvar(max_iter1);
    
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( lambda1 < 0 || 1 < lambda1 )
    {
        EOSerror->Log(FUNC, "lambda not between 0 and 1\n" );
        return 1;
    }
    lambda2 = 1. - lambda1;
    if( eos1_name == NULL || eos2_name == NULL )
    {
        EOSerror->Log(FUNC, "eos1 or eos2 not specified\n" );
        return 1;
    }
    eos1 = FetchEOS(eos1_name,*db);
    if( eos1 == NULL )
    {
        EOSerror->Log(FUNC, "FetchEOS failed for %s\n", eos1_name );
        return 1;
    }
    if( units && eos1->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "eos1->ConvertUnits(%s::%s) failed\n",
                     units->Type(),units->Name() );
        return 1;
    }   
    eos2 = FetchEOS(eos2_name,*db);
    if( eos2 == NULL )
    {
        EOSerror->Log(FUNC, "FetchEOS failed for %s\n", eos2_name );
        return 1;
    }
    if( units && eos2->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "eos2->ConvertUnits(%s::%s) failed\n",
                     units->Type(),units->Name() );
        return 1;
    }   
    V1 = eos1->V_ref;
    e1 = eos1->e_ref;
    V2 = eos2->V_ref;
    e2 = eos2->e_ref;
    P1 = P1_ref = eos1->P(V1,e1);
    T1 = T1_ref = eos1->T(V1,e1);
    P2 = P2_ref = eos2->P(V2,e2);
    T2 = T2_ref = eos2->T(V2,e2);
    if( tol_P < 0.0 )
        tol_P = TOL_P;
    if( tol_T < 0.0 )
        tol_T = TOL_T;
    if( std::isnan(V_ref) )
        V_ref = lambda1*V1+lambda2*V2;
    if( std::isnan(e_ref) )
        e_ref = lambda1*e1+lambda2*e2;
    OneDFunc_rel_tol = max(OneDFunc_rel_tol,max(eos1->OneDFunc_rel_tol,
                                                eos2->OneDFunc_rel_tol));
    eos1->OneDFunc_rel_tol = eos2->OneDFunc_rel_tol = OneDFunc_rel_tol;
    OneDFunc_abs_tol = max(OneDFunc_abs_tol,max(eos1->OneDFunc_abs_tol,
                                                eos2->OneDFunc_abs_tol));
    eos1->OneDFunc_abs_tol = eos2->OneDFunc_abs_tol = OneDFunc_abs_tol;
    ODE_epsilon = max(ODE_epsilon,max(eos1->ODE_epsilon,
                                                eos2->ODE_epsilon));
    eos1->ODE_epsilon = eos2->ODE_epsilon = ODE_epsilon;
    return 0;
}
