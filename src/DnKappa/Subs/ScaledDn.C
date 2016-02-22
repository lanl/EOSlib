#include <iostream>
using namespace std;
#include "ScaledDn.h"

ScaledDn::ScaledDn() : DnKappa("ScaledDn")
{
    c2 = c3 = c4 = c5 = 0.0;
    c6 = c8 = 0.0;
    c7 = c9 = c10 = 0.0;
    Dcj_r = lscale_r = 0.0;
    rho0_r = T0_r = 0.0;
    T0 = rho0 = lscale = 0.0;
}

ScaledDn::~ScaledDn()
{
    // Null
}

#define calcvar(var) calc.Variable(#var,&var)
#define FUNC "ScaledDn::InitParams",__FILE__,__LINE__,this
int ScaledDn::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    c2 = c3 = c4 = c5 = 0.0;
    calcvar(c2);
    calcvar(c3);
    calcvar(c4);
    calcvar(c5);
    c6 = c8 = 0.0;
    calcvar(c6);
    calcvar(c8);
    c7 = c9 = c10 = 0.0;
    calcvar(c7);
    calcvar(c9);
    calcvar(c10);
    Dcj_r = lscale_r = 0.0;
    calcvar(Dcj_r);
    calcvar(lscale_r);
    rho0_r = T0_r = 0.0;
    calcvar(rho0_r);
    calcvar(T0_r);
    T0 = rho0 = lscale = 0.0;
    calcvar(rho0);
    calcvar(T0);
    //
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    //
    if( Dcj_r < 0.0 || lscale_r < 0.0 )
    {
        EOSerror->Log(FUNC,"Dcj_r < 0 or lscale_r < 0\n");
        return 1;
    }
    if( rho0_r < 0.0 || T0_r < 0.0 )
    {
        EOSerror->Log(FUNC,"rho0_r < 0 or T0_r < 0\n");
        return 1;
    }
    if(   T0 <= 0.0 ) T0 = T0_r;
    if( rho0 <= 0.0 ) rho0 = rho0_r;
    if( InitState(rho0,T0) )
    {
        EOSerror->Log(FUNC,"InitState failed\n");
        return 1;
    }
    return 0;
}

void ScaledDn::PrintParams(ostream &out)
{
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
    DnKappa::PrintParams(out);
    out << "\t   c2  = " << c2    << "\n";
    out << "\t   c3  = " << c3    << "\n";
    out << "\t   c4  = " << c4    << "\n";
    out << "\t   c5  = " << c5    << "\n";
    out << "\t   c6  = " << c6    << "\n";
    out << "\t   c7  = " << c7    << "\n";
    out << "\t   c8  = " << c8    << "\n";
    out << "\t   c9  = " << c9    << "\n";
    out << "\t   c10 = " << c10   << "\n";
    out << "\t   Dcj_r = " <<    Dcj_r << "\n";
    out << "\tlscale_r = " << lscale_r << "\n";
    out << "\t  rho0_r = " <<   rho0_r << "\n";
    out << "\t    T0_r = " <<     T0_r << "\n";
    out << "\t  lscale = " << lscale   << "\n";
    out << "\t    rh00 = " <<   rho0   << "\n";
    out << "\t      T0 = " <<     T0   << "\n";
    //
	out.setf(old, ios::adjustfield);
}

int ScaledDn::ConvertParams(Convert &convert)
{
    if( DnKappa::ConvertParams(convert) )
        return 1;
    
    double s_T,s_vel,s_rho,s_len;
    if( !finite(s_T=convert.factor("T"))
        || !finite(s_vel=convert.factor("velocity"))
        || !finite(s_len=convert.factor("length"))
        || !finite(s_rho=convert.factor("rho")) )
            return 1;
    Dcj_r    *= s_vel;
    lscale_r *= s_len;
    T0_r     *= s_T;
    rho0_r   *= s_rho;

    lscale *= s_len;
    T0     *= s_T;
    rho0   *= s_rho;
    return 0;
}

ostream &ScaledDn::PrintComponents(ostream &out)
{
    return out;
}
