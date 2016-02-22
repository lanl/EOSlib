#include <EOS.h>
#include "Sesame.h"
#include "SesSubs.h"

#define FUNC "Sesame::InitParams",__FILE__,__LINE__,this
int Sesame::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
	delete [] sesnum;    sesnum = NULL;
	delete [] VT_name;  VT_name = NULL;
    delete [] rho_grd;  rho_grd = NULL;
    delete []  de_grd;   de_grd = NULL;
    delete []  e0_grd;   e0_grd = NULL;
    delete [] P_table;  P_table = NULL;
    delete [] T_table;  T_table = NULL;
    rho_n = e_n = 0;
    de = 0.0;
    interp = 0;
    Vcache = -1123.456789;
    EOSstatus = EOSlib::abort;

    char *ses_name = NULL;
    calc.Variable( "de", &de );
    calc.Variable( "sesnum", &ses_name );
    calc.Variable( "VT_name", &VT_name );
    calc.Variable( "interp", &interp);
	ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC, "parse failed for %s\n",line );
        return 1;
    }
    if( ses_name == NULL )
    {
        EOSerror->Log(FUNC, "sesnum not specified\n" );
        return 1;
    }
    sesnum = Strdup(ses_name);
    if( VT_name == NULL )
        VT_name = Strdup(p.name);
    if( db == NULL )
    {
        EOSerror->Log(FUNC, "db is NULL\n" );
        return 1;
    }

    EOS_VT *eos = FetchEOS_VT("Sesame",VT_name,*db);
    if( eos == NULL )
    {
        EOSerror->Log(FUNC, "Fetch SesameVT failed\n" );
        return 1;
    }
    ses_VT = static_cast<Sesame_VT*>(eos);
    if( de != 0.0 )
        ses_VT->Shift_e(-de);
    if( InvertTable(1./ses_VT->V_ref) )
    {
        EOSerror->Log(FUNC, "InvertTable failed\n" );
        return 1;
    }

    if( isnan(V_ref) )
        V_ref = ses_VT->V_ref;
    if( isnan(e_ref) )
        e_ref = ses_VT->e(V_ref, ses_VT->T_ref);
        
    EOSstatus = EOSlib::good;
    return 0;    
}


int Sesame::InvertTable(double rho_ref)
{
    rho_n = ses_VT->rho_n;
    e_n   = ses_VT->T_n;

    rho_grd = new double[rho_n];
    e0_grd  = new double[rho_n];
    de_grd  = new double[e_n];

    int nRe = rho_n*e_n;
    P_table = new double[nRe];
    T_table = new double[nRe];
    
    int i, j, k;
    for( i=0; i< rho_n; i++ )
    {
        rho_grd[i] = ses_VT->rho_grd[i];
         e0_grd[i] = ses_VT->e_table[i];
    }
    k = SESsrch(rho_ref, ses_VT->rho_grd, rho_n);
    if( k<1 )
        k=1;
    else if( k>rho_n-2 )
        k=rho_n-2;
    double e0 = ses_VT->e_table[k];
    for( j=0; j<e_n; j++ )
    {  // ToDo: check de_grd is monotonically increasing ?
         de_grd[j] = ses_VT->e_table[k]-e0;
         k += rho_n;
    }          
    double f[2];
    int one = 1;
    for( i=0; i< rho_n; i++ )
    {
        k = i;
        e0  = ses_VT->e_table[k];
        for( j=0; j<e_n; j++ )
        {
            double e = e0 + de_grd[j];
            int je = SESsrchk(e,ses_VT->e_table+i,e_n,rho_n);
            if( je<0 )
                je = 0;
            else if( je>e_n-2 )
                je = e_n-2;
            SESratfn1( e, ses_VT->e_table+i,je,e_n,rho_n,
                          ses_VT->P_table+i,rho_n, f  );
            P_table[k] = f[0];
            SESratfn1( e, ses_VT->e_table+i,je,e_n,rho_n,
                          ses_VT->T_grd,one, f  );
            T_table[k] = f[0];
            k += rho_n;         
        }
    }
    return 0;
}

EOS_VT *Sesame::VT()
{
    return ses_VT ? ses_VT->Duplicate() : NULL;
}
