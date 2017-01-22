#include <stdio.h>
#include "MatToSesame.h"

#define FUNC "MatToSesame::Ascii",__FILE__,__LINE__,this
int MatToSesame::AsciiTable(const char *file)
{
    if( EOSstatus != EOSlib::good )
        return 1;
    if( file == NULL )
        return 1;
    double *rho_vec = new double[nr];
    double *T_vec   = new double[nT];
    double *P_vec   = new double[nr*nT];
    double *e_vec   = new double[nr*nT];
    double *F_vec   = new double[nr*nT];    // F = e - T*S
    int i,j;
    double Nr = nr -1.;
    for( i=0; i<nr; i++ )
    {
        double Q = i;
        double Qc = Nr - i;
        rho_vec[i] = (Qc*rho_min + Q*rho_max)/Nr;
    }
    double NT = nT -1.;
    for( j=0; j<nT; j++ )
    {
        double Q = j;
        double Qc = NT - j;
        T_vec[j] = (Qc*T_min + Q*T_max)/NT;
    }

    int status = 0;
    int k = 0;
    for( j=0; j<nT; j++ )
    {
        double T = T_vec[j];
        for( i=0; i<nr; i++ )
        {
            double V = 1./rho_vec[i];
            if( VTeos->NotInDomain(V,T) )
            {
                P_vec[k] = EOS::NaN;
                e_vec[k] = EOS::NaN;
                F_vec[k] = EOS::NaN;
                EOSerror->Log(FUNC,"state (%g,%g) not in domain\n",V,T);
                status = 1;
            }
            else
            {
                double p = VTeos->P(V,T);
                double e = VTeos->e(V,T);
                double s = VTeos->S(V,T);
                P_vec[k] = p;
                e_vec[k] = e;
                F_vec[k] = e - T*s;
            }
            k++;
        }
    }
    if( status == 0 )
    {
        FILE *fh = fopen(file, "w");
        if( fh == NULL )
        {
            EOSerror->Log(FUNC,"could not open file %s\n",file);
            status = 1;
        }
        else
        {
            status = AsciiOut(rho_vec, T_vec, P_vec, e_vec,F_vec, fh);
            fclose(fh);
        }
    }
    delete [] F_vec;
    delete [] e_vec;
    delete [] P_vec;
    delete [] T_vec;
    delete [] rho_vec;
    return status;
}

int MatToSesame::AsciiOut(double *rho, double *T, double *P, double *e,
                          double *F, FILE *file)
{
    //char *fmt = "%15.8e"; // short
    //char *fmt = "%27.20e"; // long
    const char *fmt   = "%22.15e"; // medium
    const char *empty = "%22s";
    const char *flag[6] = { "", "10000","11000","11100","11110","11111"};
    const char *space = "";
    //
    const char *mdate = Date();
    int num = 0;
    int tbl;
    int words;
    const char* c = "r";
    //
    int i;
    // 201 table
    tbl = 201;
    words = 5;
    fprintf(file,"%2d%6d%6d%6d%4s   %6s   %6s%4i%33s%1d\n",
        num, ID, tbl, words, c, cdate,mdate,rev, space,num);
    fprintf(file,fmt,Zbar);
    fprintf(file,fmt,Abar);
    fprintf(file,fmt,rho0);
    fprintf(file,fmt,B0);
    fprintf(file,fmt,cex);
    fprintf(file,"%5s\n",flag[words]);
    // 301 table
    num=1;
    tbl=301;
    words = nr + nT + 3*nr*nT+2;
    fprintf(file,"%2d%6d%6d%6d%4s   %6s   %6s%4i%33s%1d\n",
        num, ID, tbl, words, c, cdate,mdate,rev, space,num);
    //
    fprintf(file,fmt,double(nr));
    fprintf(file,fmt,double(nT));
    int count = 2;
    for( i=0; i<nr; i++ )
    {
        fprintf(file,fmt,rho[i]);
        count++;
        if( count == 5 )
        {
            fprintf(file,"%5s\n",flag[count]);
            count = 0;
        }
    }
    for( i=0; i<nT; i++ )
    {
        fprintf(file,fmt,T[i]);
        count++;
        if( count == 5 )
        {
            fprintf(file,"%5s\n",flag[count]);
            count = 0;
        }
    }
    int nrT = nr*nT;
    for( i=0; i<nrT; i++ )
    {
        fprintf(file,fmt,P[i]);
        count++;
        if( count == 5 )
        {
            fprintf(file,"%5s\n",flag[count]);
            count = 0;
        }
    }
    for( i=0; i<nrT; i++ )
    {
        fprintf(file,fmt,e[i]);
        count++;
        if( count == 5 )
        {
            fprintf(file,"%5s\n",flag[count]);
            count = 0;
        }
    }
    for( i=0; i<nrT; i++ )
    {
        fprintf(file,fmt,F[i]);
        count++;
        if( count == 5 )
        {
            fprintf(file,"%5s\n",flag[count]);
            count = 0;
        }
    }
    // last line
    if( count > 0 )
    {
        for( i=count+1; i<=5; i++ )
            fprintf(file,empty,space);
        fprintf(file,"%5s\n",flag[count]);
    }
    // end of tables
    num = 2;
    fprintf(file,"%2d%76s%2d\n",num,space,num);
    
    return 0;
}
