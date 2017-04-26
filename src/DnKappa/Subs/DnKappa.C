#include "DnKappa.h"

DnKappa::~DnKappa()
{
    // Null
}

DnKappa::DnKappa(const char *Dn_type) : EOSbase("DnKappa",Dn_type)
{
    if( !init )
        Init();
    kappa_max = 0.0;
    omega_s   = 0.0;
    Dcj       = 0.0;
    d_max     = 0.0;
}

DnKappa::DnKappa(const DnKappa &DnK) : EOSbase(DnK)
{
    kappa_max = DnK.kappa_max;
    omega_s   = DnK.omega_s;
    Dcj       = DnK.Dcj;
    d_max     = DnK.d_max;
}

int DnKappa::Copy(const DnKappa& DnK, int check)
{
    if( EOSbase::Copy(DnK,check) )
        return 1;
    kappa_max = DnK.kappa_max;
    omega_s   = DnK.omega_s;
    Dcj       = DnK.Dcj;
    d_max     = DnK.d_max;
    return 0;
}

#define FUNC "DnKappa::InitBase", __FILE__, __LINE__, this
#define calcvar(var) calc.Variable(#var,&var)
int DnKappa::InitBase(Parameters &p, Calc &calc, DataBase *db)
{
    kappa_max = 0.0;
    omega_s   = 0.0;
    Dcj       = 0.0;
    d_max     = 1.2;
    calcvar(kappa_max);
    calcvar(omega_s);
    calcvar(Dcj);
    EOSstatus = EOSlib::abort;
    if( InitParams(p, calc, db) )
    {
        EOSerror->Log(FUNC,"Initialize for %s failed\n", type);
        return 1;
    }
    if( Dcj < 0.0 || kappa_max < 0.0  )
    {
        EOSerror->Log(FUNC,"Dcj or kappa_max not set\n");
        return 1;
    }
    EOSstatus = EOSlib::good;
    return 0;    
}

void  DnKappa::PrintParams(ostream &out)
{
    // derived class overloads function
    out << "\tkappa_max = " << kappa_max << "\n";
    out << "\tomega_s   = " << omega_s   << "\n";
    out << "\tDcj       = " << Dcj       << "\n";
    out << "\td_max     = " << d_max     << "\n";
}

int DnKappa::ConvertParams(Convert &convert)
{
    if( EOSstatus != EOSlib::good )
        return 1;
    double s_len;
    double s_vel;
    if( !finite(s_len=convert.factor("length"))
        || !finite(s_vel=convert.factor("velocity")) )
            return 1;
    kappa_max /= s_len;
    Dcj *= s_vel;
    return 0;
}

int DnKappa::InitState(double, double)
{
    // derived class may overload function
    return 1;
}
