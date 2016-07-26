#include <Arg.h>

#include "ExtEOSmanager.h"
#include "Riemann_genX.h"

#define NaN EOS_VT::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    //const char *file = "Test.data";
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ElasticPlasticTest.data";
    const char * file = file_.c_str();
    
    const char *mat1 = "VonMises::HMX";
    const char *mat2 = "Hayes::HMX";
    const char *units = "std";
    const char *lib = "../../lib/Linux";
    const char *logfile = "/dev/null";
    double Ps = 1;

    EOS::Init();
    
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(lib,lib);
        GetVar(logfile,logfile);
        
        GetVar(mat1,mat1);
        GetVar(mat2,mat2);
        GetVar(units,units);
        GetVar(Ps,Ps);
            
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


    int i1 = materials.index_eos(mat1,units);
    if( i1 < 0 )
        cerr << Error("FetchEOS failed for ") << mat1 << Exit;
    ExtEOS &xeos1 = materials.xeos(i1);
    ExtEOSstate *state1 = new ExtEOSstate(xeos1);
    
    int i2 = materials.index_eos(mat2,units);
    if( i2 < 0 )
        cerr << Error("FetchEOS failed for ") << mat2 << Exit;
    ExtEOS &xeos2 = materials.xeos(i2);
    ExtEOSstate *state2 = new ExtEOSstate(xeos2);

    cout << "State1\n" << *state1 << "\n";
    cout << "State2\n" << *state2 << "\n";
    
    
    Hugoniot *H1 = xeos1.shock(*state1,state1->z);
    if( H1==NULL )
        cerr << Error("H1 is NULL ") << Exit;
	WaveState s1;
	if( H1->P(Ps,RIGHT,s1) )
		cerr << Error("H1->P failed") << Exit;
    ExtEOSstate *shock1 = new ExtEOSstate(xeos1,s1,state1->z);
    cout << "shock1\n" << *shock1 << "\n";
    
    RiemannSolver_genX RS(xeos1, shock1->z, xeos2, state2->z);
    WaveState wave1, wave2;
    if( RS.Solve(*shock1,*state2,wave1,wave2) )
        cerr << Error("RP.Solve failed") << Exit;

    WaveStateLabel(cout)<< "\n";
    cout << wave1 << "\n";
    cout << wave2 << "\n";
    
    return 0;
}

