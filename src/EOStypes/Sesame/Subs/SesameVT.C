#include <EOS_VT.h>
#include "Sesame_VT.h"
#include <iostream>
using namespace std;

char *Sesame_VT::DATA = NULL;

Sesame_VT::Sesame_VT() : EOS_VT("Sesame")
{
    sesnum = NULL;
    rho_n = T_n = 0;
    rho_grd = T_grd = NULL;
    P_table = e_table = NULL;
    S_table = NULL;
    Vcache = Tcache = -1123.456789;
    interp = 0;
}


Sesame_VT::~Sesame_VT()
{
	delete [] sesnum;
    delete [] rho_grd;
    delete []   T_grd;
    delete [] P_table;
    delete [] e_table;
    delete [] S_table;
}


int Sesame_VT::SesDir(DataBase &db)
{
    if( DATA )
        return 0;
    Parameters *Env = db.FetchParams(NULL,"DataBase","Environment");
    if( Env == NULL )
        return 1;
    DATA = Strdup( Env->Value("SesameData") );
    return DATA == NULL;   
}



void Sesame_VT::PrintParams(ostream &out)
{
    EOS_VT::PrintParams(out);
    if( sesnum )
        out << "\tsesnum = " << sesnum << "\n";
    out << "\tinterp = " << interp << "\n";
    if( rho_n > 0 )
        out << "\t# rho_n = " << rho_n << ", T_n = " << T_n << "\n";
    if( rho_grd )
        out << "\t# rho_min = " << rho_grd[0]
            << ", rho_max = "   << rho_grd[rho_n-1] << "\n";
    if( T_grd )
        out << "\t# T_min = " << T_grd[0]
            << ", T_max = "   << T_grd[T_n-1] << "\n";
}

int Sesame_VT::ConvertParams(Convert &convert)
{
    Vcache = Tcache = -1123.456789;  // invalidate cache
    int status = EOS_VT::ConvertParams(convert);
    if( status )
        return status;
	double s_V, s_e, s_T;
	if(  !finite(s_V = convert.factor("V"))
	      || !finite(s_e = convert.factor("e"))
	          || !finite(s_T = convert.factor("T")) )
	{
	    return 1;
	}
    double s_rho = 1/s_V;
    double s_P   = s_e/s_V;
    double s_S   = s_e/s_T;

    int i;
    for( i=0; i<rho_n; i++ )
        rho_grd[i] *= s_rho;
    for( i=0; i<T_n; i++ )
        T_grd[i] *= s_T;
    int nRT = rho_n*T_n;
    for( i=0; i<nRT; i++ )
        P_table[i] *= s_P;
    for( i=0; i<nRT; i++ )
        e_table[i] *= s_e;
    for( i=0; i<nRT; i++ )
        S_table[i] *= s_S;
    
    return 0;
}
