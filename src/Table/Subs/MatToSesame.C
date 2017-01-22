#include <iostream>
#include <iomanip>
using namespace std;
#include "MatToSesame.h"

MatToSesame::MatToSesame() : Table("MatToSesame")
{
    mat = NULL;
    eos = NULL;
    VTeos = NULL;
    cdate = NULL;
}

MatToSesame::~MatToSesame()
{
    deleteEOS_VT(VTeos);
    deleteEOS(eos);
    delete [] mat;
    delete [] cdate;
}

#define calcvar(var) calc.Variable(#var,&var)
#define FUNC "MatToSesame::InitParams",__FILE__,__LINE__,this
int MatToSesame::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
    deleteEOS_VT(VTeos);
    deleteEOS(eos);
    delete [] mat;
    mat = NULL;
    delete [] cdate;
    cdate = NULL;
    rev = 0;
    ID = 0;
    calcvar(mat);
    calcvar(cdate);
    calcvar(rev);
    calcvar(ID);
    //
    Zbar = 0.0;
    Abar = 0.0;
    rho0 = 0.0;
    B0   = 0.0;
    cex  = 0.0;
    calcvar(Zbar);
    calcvar(Abar);
    calcvar(rho0);
    calcvar(B0);
    calcvar(cex);
    nr = 0;
    rho_min = -1.;
    rho_max = -1.;
    nT = 0;
    T_min = -1.;
    T_max = -1.;
    calcvar(nr);
    calcvar(rho_min);
    calcvar(rho_max);
    calcvar(nT);
    calcvar(T_min);
    calcvar(T_max);
    double drho = 0.0;
    double dT   = 0.0;
    calcvar(drho);
    calcvar(dT);
    //
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    //
    if( rho_min <= 0.0 || rho_max <= rho_min )
    {
        EOSerror->Log(FUNC,"invalid rho_min (%lg) or rho_max (%lg)\n",
                             rho_min, rho_max);
        return 1;
    }
    if( nr < 1 )
    {
        if( drho <= 0.0 )
        {
            EOSerror->Log(FUNC,"invalid nr or drho\n");
            return 1;
        }
        nr = int((rho_max - rho_min)/drho+0.5) + 1;
    }
    //
    if( T_min < 0.0 || T_max <= T_min )
    {
        EOSerror->Log(FUNC,"invalid T_min (%lg) or T_max (%lg)\n",
                             T_min, T_max);
        return 1;
    }
    if( nT < 1 )
    {
        if( dT <= 0.0 )
        {
            EOSerror->Log(FUNC,"invalid nT or dT\n");
            return 1;
        }
        nT = int((T_max - T_min)/dT+0.5) + 1;
    }
    //
    if( mat == NULL )
    {
        EOSerror->Log(FUNC,"mat not specified\n");
        return 1;
    }
    eos = FetchEOS(mat, *db);
    if( eos == NULL )
    {
        EOSerror->Log(FUNC,"FetchEOS failed for mat %s\n",mat);
        return 1;
    }
    if( units && eos->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "eos->ConvertUnits(%s::%s) failed\n",
                     units->Type(),units->Name() );
        return 1;
    }
    VTeos = eos->eosVT();
    if( VTeos == NULL )
    {
        EOSerror->Log(FUNC,"eosVT failed for mat %s\n",mat);
        return 1;
    }
    if( rho0 <= 0. )
        rho0 = 1./VTeos->V_ref;
    if( B0 <= 0.0 )
        B0 = VTeos->KT(1./rho0,VTeos->T_ref);
    //
    if( ID <= 0 || ID > 999999 )
    {
        EOSerror->Log(FUNC,"invalid ID (%d)\n",ID);
        return 1;
    }
    if( cdate == NULL )
        cdate = Date();
    return 0;
}

void MatToSesame::PrintParams(ostream &out)
{
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
    if( mat )
	    out << "\t  mat = " << mat   << "\n";
    if( cdate )
	    out << "\tcdate = " << cdate << "\n";
    out << "\t  rev = " << rev   << "\n";
    out << "\t   ID = " << setw(6) << ID << "\n";
    //
    out << "\t Zbar = " << Zbar  << "\n";
    out << "\t Abar = " << Abar  << "\n";
    out << "\t rho0 = " << rho0  << "\n";
    out << "\t   B0 = " << B0    << "\n";
    out << "\t  cex = " << cex   << "\n";
    //
    out << "\t      nr = " << nr      << "\n";
    out << "\t rho_min = " << rho_min << "\n";
    out << "\t rho_max = " << rho_max << "\n";
    out << "\t      nT = " << nT      << "\n";
    out << "\t   T_min = " << T_min   << "\n";
    out << "\t   T_max = " << T_max   << "\n";
    //
	out.setf(old, ios::adjustfield);
}

int MatToSesame::ConvertParams(Convert &convert)
{
    if( Table::ConvertParams(convert) )
        return 1;
    
    double s_T,s_P,s_rho;
    if( !finite(s_T=convert.factor("T"))
        || !finite(s_P=convert.factor("P"))
        || !finite(s_rho=convert.factor("rho")) )
            return 1;
    rho0 *= s_rho;
    B0   *= s_P;
    // cex ?
    rho_min *= s_rho;
    rho_max *= s_rho;
    T_min *= s_T;
    T_max *= s_T;

    if( eos==NULL || eos->ConvertUnits(convert) )
        return 1;
    if( VTeos==NULL || VTeos->ConvertUnits(convert) )
        return 1;

    return 0;
}

ostream &MatToSesame::PrintComponents(ostream &out)
{
    if( eos )
        eos->PrintAll(out);
    //if( VTeos )
    //    VTeos->PrintAll(out);
    return out;
}
