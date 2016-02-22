#include "EOS.h"
#include "EOS_VT.h"

int EOS_VT::init = 0;

void EOS_VT::Init()
{
    if( init )
        return;
    Initbase();
    init = 1;
}

EOS_VT::EOS_VT(const char *eos_type) : EOSbase("EOS_VT",eos_type), Ve_eos(NULL)
{
    if( !init )
        Init();
    
    V_ref = NaN;
    T_ref = NaN;
}

EOS_VT::EOS_VT(const EOS_VT &eos) : EOSbase(eos), Ve_eos(NULL)
{
    V_ref = eos.V_ref;
    T_ref = eos.T_ref;
}

int EOS_VT::Copy(const EOS_VT &eos, int check)
{
    if( EOSbase::Copy(eos,check) )
        return 1;
    V_ref = eos.V_ref;
    T_ref = eos.T_ref;
    deleteEOS(Ve_eos);
    return 0;
}

void  EOS_VT::PrintParams(ostream &out)
{
    if( finite(V_ref) || finite(T_ref) )
    {
        out << "\tV_ref = " << V_ref << "; T_ref = " << T_ref << "\n";
        double P_ref = P(V_ref,T_ref);
        double e_ref = e(V_ref,T_ref);
        out << "#\tP_ref = " << P_ref << "; e_ref = " << e_ref << "\n";
    }
}

int EOS_VT::ConvertParams(Convert &convert)
{
    double s_V, s_T;
    if( !finite(s_V=convert.factor("V")) || !finite(s_T=convert.factor("T")) )
        return 1;
    V_ref *= s_V;
    T_ref *= s_T;
    if( Ve_eos )
    {
        if( Ve_eos->ConvertUnits(convert) )
            deleteEOS(Ve_eos);
    }
    return 0;   

}

#define FUNC "EOS_VT::Initialize", __FILE__, __LINE__, this
#define calcvar(var) calc.Variable(#var,&var)
int EOS_VT::InitBase(Parameters &p, Calc &calc, DataBase *db)
{
    deleteEOS(Ve_eos);
    V_ref = T_ref = NaN;
    CalcVar *rho_ref = new CVpinvdouble(&V_ref);
    calcvar(V_ref);
    calcvar(T_ref);
    calc.Variable("rho_ref", rho_ref);

    if( InitParams(p, calc, db) )
    {
        EOSerror->Log(FUNC,"Initialize for %s failed\n", type);
        return 1;
    }

    EOSstatus = EOSlib::good;
    if( isnan(V_ref) || isnan(T_ref) )
    {
        EOSstatus = EOSlib::bad;
        EOSerror->Log(FUNC,"ref state undefined: V_ref=%lf, T_ref=%lf\n",V_ref,T_ref);
        return 1;
    }
    return 0;    
}
