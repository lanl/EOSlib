#include "PTequilibrium_VT.h"
#include "PTequilibrium.h"

PTequilibrium_VT::PTequilibrium_VT() : EOS_VT("PTequilibrium")
{
    eos1 = eos2 = NULL;
    lambda1 = lambda2 = -1.;
    de = 0.0;
    V1 = V2 = 0.0;
    tol = 1e-6;
}

PTequilibrium_VT::PTequilibrium_VT(PTequilibrium &eos) : EOS_VT("PTequilibrium")
{
    Transfer(eos);
    lambda1 = eos.lambda1;
    lambda2 = eos.lambda2;
    de      = eos.de;
    tol = eos.tol;
    V1 = V2 = 0.0;
    if( eos.eos1 == NULL || eos.eos2 == NULL )
    {
        eos1 = eos2 = NULL;
        EOSstatus = EOSlib::abort;
        return;
    }
    eos1 = eos.eos1->eosVT();
    eos2 = eos.eos2->eosVT();
    if( eos1 && eos2 )
        EOSstatus = EOSlib::good;
    else
    {
	    deleteEOS_VT(eos1);
	    deleteEOS_VT(eos2);   
        eos1 = eos2 = NULL;
        EOSstatus = EOSlib::abort;
    }
}

PTequilibrium_VT::~PTequilibrium_VT()
{
	deleteEOS_VT(eos1);
	deleteEOS_VT(eos2);   
}

EOS *PTequilibrium_VT::Ve()
{
    return new PTequilibrium(*this);
}

void PTequilibrium_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
    if( eos1 )
        out << "\teos1 = \"" << eos1->Type() << "::" << eos1->Name() << "\"\n";
    if( eos2 )
        out << "\teos2 = \"" << eos2->Type() << "::" << eos2->Name() << "\"\n";
    out << "\tlambda1 = " << lambda1 << "\n";
    out << "\tde      = " << de      << "\n";
    out << "\ttol     = " << tol     << "\n";
}
ostream &PTequilibrium_VT::PrintComponents(ostream &out)
{
    if( eos1 )
        eos1->PrintAll(out);
    if( eos2 )
        eos2->PrintAll(out);
    return out;
}

int PTequilibrium_VT::ConvertParams(Convert &convert)
{
    if( EOSstatus != EOSlib::good )
        return 1;
    V1 = V2 = 0.0;
    if( EOS_VT::ConvertParams(convert)
        || eos1->ConvertUnits(convert)
        || eos2->ConvertUnits(convert) )
    {
        EOSstatus = EOSlib::bad;
        return -1;
    }
    double s_e = convert.factor("e");
    if( !finite(s_e) )
    {
        EOSstatus = EOSlib::bad;
        return -1;
    }
    de  *= s_e;
    return 0;
}

#define FUNC "PTequilibrium_VT::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int PTequilibrium_VT::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    deleteEOS_VT(eos1);
    deleteEOS_VT(eos2);
    lambda1 = lambda2 = -1;
    de  = 0.0;
    tol = 1e-6;
    char *eos1_name = NULL;
    calc.Variable( "eos1", &eos1_name );
    char *eos2_name = NULL;
    calc.Variable( "eos2", &eos2_name );
    calcvar(lambda1);
    calcvar(de);
    calcvar(tol);
    
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( lambda1 < 0 || 1 < lambda1 )
    {
        EOSerror->Log(FUNC, "lambda1 not between 0 and 1\n" );
        return 1;
    }
    lambda2 = 1. - lambda1;
    if( eos1_name == NULL || eos2_name == NULL )
    {
        EOSerror->Log(FUNC, "eos1 or eos2 not specified\n" );
        return 1;
    }
    eos1 = FetchEOS_VT(eos1_name,*db);
    if( eos1 == NULL )
    {
        EOSerror->Log(FUNC, "FetchEOS_VT failed for %s\n", eos1_name );
        return 1;
    }
    if( units && eos1->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "eos1->ConvertUnits(%s::%s) failed\n",
                     units->Type(),units->Name() );
        return 1;
    }   
    eos2 = FetchEOS_VT(eos2_name,*db);
    if( eos2 == NULL )
    {
        EOSerror->Log(FUNC, "FetchEOS_VT failed for %s\n", eos2_name );
        return 1;
    }
    if( units && eos2->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "eos2->ConvertUnits(%s::%s) failed\n",
                     units->Type(),units->Name() );
        return 1;
    }   
    if( isnan(V_ref) )
        V_ref = lambda1*eos1->V_ref+lambda2*eos2->V_ref;
    if( isnan(T_ref) )
        T_ref = max(eos1->T_ref, eos2->T_ref);
    return 0;
}
