#include <Arg.h>
#include <fstream>

#include "ElasticPlastic.h"
#include "ExtEOSmanager.h"

#define NaN EOS_VT::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "Test.data";
    const char *type = "VonMises";
    const char *name = "HMX";
    const char *units = "std";
    const char *lib = "../../lib/Linux";
    const char *logfile = "/dev/null";

    EOS::Init();
    double V = NaN;
    double e = NaN;
    double eps_pl = -0.1;   // negative in compression
    
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(lib,lib);
        GetVar(logfile,logfile);

        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        GetVar(V,V);
        GetVar(e,e);
        GetVar(eps_pl,eps_pl);
        
        ArgError;
    }

    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;

    ofstream log(logfile);
    ExtEOSmanager materials(log);
    char *SharedLibDir = Cat( "SharedLibDirEOS=",lib );
    materials.SetEnv(SharedLibDir);
    delete [] SharedLibDir;
    if( materials.Read(file) )
        cerr << Error("error reading file ") << file << Exit;

    char *mat_name = Cat( type, "::", name );
    int i = materials.index_eos(mat_name,units);
    if( i < 0 )
        cerr << Error("FetchEOS failed") << Exit;
    ExtEOS &xeos = materials.xeos(i);
    
    if( !isnan(V) && !isnan(e) )
    {
        double P = xeos.P(V,e);
        cout << "P = " << P << "\n";
        double T = xeos.T(V,e);
        cout << "T = " << T << "\n";
        double S = xeos.S(V,e);
        cout << "S = " << S << "\n";
        double c = xeos.c(V,e);
        cout << "c = " << c << "\n";

        int n = xeos.N();
        cout << "n = " << n << "\n";
        double *z = new double[n];
        double *zdot = new double[n];
        z[0] = eps_pl;
        if( xeos.Rate(V,e,z,zdot) )
            cerr << Error("Rate failed") << Exit;
        cout << "Rate = " << zdot[0] << "\n";

    }
    else
    {
	    xeos.PrintAll(cout);
        ExtEOSstate state(xeos );
        cout << "ref_state\n" << state;
    }
    return 0;
}

