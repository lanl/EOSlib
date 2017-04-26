#include "EOS.h"
#include "EOS_VT.h"

int EOS::init = 0;

void EOS::Init()
{
    if( init )
        return;
    Initbase();
    InitFormat();  
    init = 1;
}

EOS::EOS(const char *eos_type) : EOSbase("EOS",eos_type), VTeos(NULL)
{
    if( !init )
        Init();
    
    V_ref = NaN;
    e_ref = NaN;
}

EOS::EOS(const EOS &eos) : EOSbase(eos), VTeos(NULL)
{
    *(EOS_tol*)this = (EOS_tol&)eos;
    V_ref = eos.V_ref;
    e_ref = eos.e_ref;
}

int EOS::Copy(const EOS& eos, int check)
{
    if( EOSbase::Copy(eos,check) )
        return 1;
    *(EOS_tol*)this = (EOS_tol&)eos;
    V_ref = eos.V_ref;
    e_ref = eos.e_ref;
    deleteEOS_VT(VTeos);
    return 0;
}

void EOS::SetVerbose(int d)
{
    verbose = d;
}

void  EOS::PrintParams(ostream &out)
{
    if( finite(V_ref) || finite(e_ref) )
    {
        out << "\tV_ref = " << V_ref << "; e_ref = " << e_ref << "\n";
        double P_ref = P(V_ref,e_ref);
        double T_ref = T(V_ref,e_ref);
        out << "#\tP_ref = " << P_ref << "; T_ref = " << T_ref << "\n";
    }
}

int EOS::ConvertParams(Convert &convert)
{
    if( status() != EOSlib::good )
        return 1;
    double s_V, s_e;
    if( !finite(s_V=convert.factor("V")) || !finite(s_e=convert.factor("e"))
            || Convert_tol(convert) )
        return 1;
	V_ref *= s_V;
	e_ref *= s_e;
    if( VTeos )
    {
        if( VTeos->ConvertUnits(convert) )
            deleteEOS_VT(VTeos);
    }
    return 0;   

}

#define FUNC "EOS::InitBase", __FILE__, __LINE__, this
#define calcvar(var) calc.Variable(#var,&var)
int EOS::InitBase(Parameters &p, Calc &calc, DataBase *db)
{
    deleteEOS_VT(VTeos);
    EOS_tol::InitParams(calc);
    V_ref = e_ref = NaN;
    CalcVar *rho_ref = new CVpinvdouble(&V_ref);
    calcvar(V_ref);
    calcvar(e_ref);
    calc.Variable("rho_ref", rho_ref);
    double T_ref  = 300;
    double P_ref  = NaN;
    calcvar(T_ref);
    calcvar(P_ref);

    EOSstatus = EOSlib::abort;
    if( InitParams(p, calc, db) )
    {
        EOSerror->Log(FUNC,"Initialize for %s failed\n", type);
        return 1;
    }

    EOSstatus = EOSlib::good;
    if( !std::isnan(P_ref) && !std::isnan(T_ref) )
    {
        HydroState state;
        if( this->PT(P_ref,T_ref,state) )
        {
            EOSstatus = EOSlib::bad;
            EOSerror->Log(FUNC,"PT(%lf,%lf) failed\n",P_ref,T_ref);
            return 1;
        }
        V_ref = state.V;
        e_ref = state.e;
    }
    else if( std::isnan(V_ref) || std::isnan(e_ref) )
    {
        EOSstatus = EOSlib::bad;
        EOSerror->Log(FUNC,"ref state undefined: V_ref=%lf, e_ref=%lf\n",V_ref,e_ref);
        return 1;
    }
    return 0;    
}
