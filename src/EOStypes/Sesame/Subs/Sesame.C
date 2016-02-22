#include <EOS.h>
#include "Sesame.h"

Sesame::Sesame() : EOS("Sesame")
{
	sesnum = NULL;
    ses_VT = NULL;
    VT_name = NULL;
    interp = 0;
    de = 0.0;

    rho_n = e_n = 0;
    rho_grd = de_grd = e0_grd = NULL;
    P_table = T_table = NULL;

    Vcache = -1123.456789;
}

Sesame::~Sesame()
{
    delete [] sesnum;
    delete [] VT_name;
    EOS_VT *eptr = static_cast<EOS_VT*>(ses_VT);
    deleteEOS_VT(eptr);
    delete [] rho_grd;
    delete [] de_grd;
    delete [] e0_grd;
    delete [] P_table;
    delete [] T_table;
}

void Sesame::PrintParams(ostream &out)
{
    EOS::PrintParams(out);
    if( sesnum )
        out << "\t sesnum = \"" << sesnum << "\"\n";
    if( VT_name && name && strcmp(VT_name,name) )
        out << "\tVT_name =  \"" << VT_name << "\"\n";    
    out << "\t interp = " << interp << "\n";
    if( de != 0.0 )
        out << "\t     de = " << de << "\n";
    if( rho_n > 0 )
        out << "\t# rho_n = " << rho_n << ", e_n = " << e_n << "\n";
    if( rho_grd )
        out << "\t# rho_min = " << rho_grd[0]
            << ", rho_max = "   << rho_grd[rho_n-1] << "\n";
    if( de_grd )
        out << "\t# de_min = " << de_grd[0]
            << ", de_max = "   << de_grd[e_n-1] << "\n";
    if( e0_grd )
        out << "\t# e0_min = " << e0_grd[0]
            << ", e0_max = "   << e0_grd[rho_n-1] << "\n";
}

int Sesame::ConvertParams(Convert &convert)
{
    Vcache = -1123.456789;  // invalidate cache
    int status = EOS::ConvertParams(convert);
    if( status )
        return status;
    status = ses_VT->ConvertParams(convert);
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

    int i;
    for( i=0; i<rho_n; i++ )
        rho_grd[i] *= s_rho;
    de *= s_e;
    for( i=0; i<rho_n; i++ )
        e0_grd[i] *= s_e;
    for( i=0; i<e_n; i++ )
        de_grd[i] *= s_e;
    int nRe = rho_n*e_n;
    for( i=0; i<nRe; i++ )
        P_table[i] *= s_P;
    for( i=0; i<nRe; i++ )
        T_table[i] *= s_T;
    
    return 0;
}
