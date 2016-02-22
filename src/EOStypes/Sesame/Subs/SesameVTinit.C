#include <iostream>
#include <fstream>
using namespace std;
#include <EOS_VT.h>
#include "Sesame_VT.h"

static void SkipLine(istream &in)
{
    int c;
    while( (c=in.get()) != EOF )
    {
        if( c == '\n' )
            break;
    }

}

#define FUNC "Sesame_VT::InitParams",__FILE__,__LINE__,this
int Sesame_VT::InitParams(Parameters &p, Calc &calc, DataBase *db)
{
	delete [] sesnum;    sesnum = NULL;
    delete [] rho_grd;  rho_grd = NULL;
    delete []   T_grd;    T_grd = NULL;
    delete [] P_table;  P_table = NULL;
    delete [] e_table;  e_table = NULL;
    delete [] S_table;  S_table = NULL;
    rho_n = T_n = 0;
    interp = 0;
    Vcache = Tcache = -1123.456789;
    EOSstatus = EOSlib::abort;
    char *ses_name = NULL;
    calc.Variable( "sesnum", &ses_name );
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
    if( DATA == NULL )
    {
        if( db == NULL )
        {
            EOSerror->Log(FUNC, "db is NULL\n" );
            return 1;
        }            
        if( SesDir(*db) )
        {
            EOSerror->Log(FUNC, "SESAME DATA Directory not defined\n" );
            return 1;
        }
    }
    char *dir = Cat(DATA,"/",sesnum,"/301");
  //
    char *file = Cat(dir,"/item_directory");
    ifstream in(file);
    if( !in )
    {
        EOSerror->Log(FUNC, "open failed for file %s\n", file );
        return 1;
    }
    int n1, n2;
    char item[128];
    char type[128];
    while( (in >> item >> type >> n1 >> n2) )
    {
        if( !strcmp(item,"R") )
            rho_n = n1;
        else if( !strcmp(item,"T") )
            T_n = n1;
            
        SkipLine(in);
    }
    delete [] file;
    if( rho_n <= 0 || T_n <= 0 )
    {
        EOSerror->Log(FUNC, "rho_n, T_n = %d, %d\n", rho_n, T_n );
        return 1;
    }
  //
    rho_grd = new double[rho_n];
      T_grd = new double[T_n];

    int nRT = rho_n*T_n;
    P_table = new double[nRT];
    e_table = new double[nRT];
  //           
    file = Cat(dir,"/R");
    if( ReadArray(file, rho_grd, rho_n) )
        return 1;
    delete [] file;
    
    file = Cat(dir,"/T");
    if( ReadArray(file, T_grd, T_n) )
        return 1;
    delete [] file;
    
    file = Cat(dir,"/P");
    if( ReadArray(file, P_table, nRT) )
        return 1;
    delete [] file;
  
    file = Cat(dir,"/E");
    if( ReadArray(file, e_table, nRT) )
        return 1;
    delete [] file;
    
    S_table = new double[nRT];
    ComputeS();
  
    delete [] dir;
    return 0;    
}


#undef FUNC
#define FUNC "Sesame_VT::ReadArray",__FILE__,__LINE__,this
int Sesame_VT::ReadArray(const char *file, double *array, int n)
{
    ifstream in(file);
    if( !in )
    {
        EOSerror->Log(FUNC, "open failed for file %s\n", file );
        return 1;
    }
    int i;
    for( i=0; i < n; i++ )
    {
        if( !(in >> array[i]) )
        {
            EOSerror->Log(FUNC, "too few elements in file %s, expected %d, read %d\n",
                    file, n, i );
            return 1;
        }
    }
    double dummy;
    if( (in>>dummy) )
    {
        EOSerror->Log(FUNC,"too many elements in file %s, expected %d\n",file,n);
        return 1;
    }
    return 0;
}

void Sesame_VT::ComputeS()
{ // de = P/rho^2 drho + TdS
    int i;          // density index
    int j=0;        // T index
    int k=0;        // [density,T] index
    int i1 = (rho_grd[0] == 0.0) ? 1 : 0;
    double sj = 0.0;            // sj = S[i1,j]
    double Tj = T_grd[0];
    double ej = e_table[i1];    // ej = e[i1,j]
    if( Tj == 0.0 )
    {   // zero isotherm is zero isentrope
        for( i=0; i<rho_n; i++)
            S_table[i] = 0.0;
        j = 1;
        k = rho_n;
    }
    for( ; j<T_n;   j++)
    {
        double T0 = Tj;
        double e0 = ej;
        // integrate dS = de/T, for first non-zero rho grid-line
        Tj = T_grd[j];
        ej = e_table[k+i1];     // e[i1,j]
        sj += 2.*(ej-e0)/(Tj+T0);
        S_table[k++] = sj;      // S[0,j]
        if( i1 == 1 )
            S_table[k++] = sj;  // S[1,j] = S[0,j] if rho_grd[0] = 0            
        // integrate dS = (de - P/rho^2 drho)/T, along T grid-line
        double s = sj;          // s = S[i,j] starting at i=i1
        for( i=i1+1; i<rho_n; i++)
        {
            double de = e_table[k] - e_table[k-1];
            double drho = rho_grd[i]-rho_grd[i-1];
            double Pav = 0.5*(  P_table[k]/sqr(rho_grd[i])
                              + P_table[k-1]/sqr(rho_grd[i-1]) );
            s += (de -Pav*drho)/Tj;
            S_table[k++] = s;
        }
    }
}

void Sesame_VT::Shift_e(double de)
{
    if( de == 0.0 || e_table == NULL )
        return;
    int nRT = rho_n*T_n;
    int k;
    for( k=0; k<nRT; ++k )
        e_table[k] += de;
}
