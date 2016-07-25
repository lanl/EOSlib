#include <Arg.h>
#include "EOSmanager.h"
#include "ExtEOSmanager.h"
using namespace std;

int main(int, char **argv)
{
	ProgName(*argv);
//	const char *file     = "Test.data";
	std::string file_;
        file_ = getenv("EOSLIB_DATA_PATH");
	file_ += "/test_data/ExtEOStypesTest.data";
	const char * file = file_.c_str();
        const char *uname    = "std";
//	const char *lib      = "../lib/Linux";
        std::string libPath;
        libPath  = getenv("EOSLIB_SHARED_LIBRARY_PATH");
        const char * lib     = libPath.c_str();
        const char *material = "StiffIdealHEav::PBX9501";
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
        GetVar(uname,uname);
        GetVar(lib,lib);
        GetVar(material,material);
		ArgError;
	}
    //
    // initialize database
    //
    ofstream EOS_log("/dev/null");
    ExtEOSmanager manager(EOS_log);
    char *SharedLibDir = Cat( "SharedLibDirEOS=",lib );
    manager.SetEnv(SharedLibDir);
    delete [] SharedLibDir;
    if( manager.Read(file) )
        cerr << Error("error reading file ") << file << Exit;
    if( manager.Read(file) )
        cerr << Error("error reading file ") << file << Exit;
    //
    // Fetch material and allocate reference state
    //
    int m = manager.index_eos(material,uname);
    if( m < 0 )
        cerr << Error("could not find material ") << material << Exit;
    ExtEOS &Xeos = manager.xeos(m);
    Xeos.Print(cout) << "\n";
    ExtEOSstate *nstate = new ExtEOSstate(Xeos);
    cout << "nstate\n" << *nstate << "\n";
    char *handle = "refstate";
    int s = manager.AddState(handle, nstate); // nstate set to null
    ExtEOSstate &State = manager.state(s);
    cout << "State\n" << State << "\n";
    //
    //   AMRITA EOS plugin keyword state at CJ
    //
    ExtEOSstate *state = new ExtEOSstate(State);
    HydroState hstate(state->V,state->e,state->u);
    Detonation *det = Xeos.detonation(hstate,state->P,state->z);
    WaveState wstate;
    if( det == NULL || det->CJwave(RIGHT,wstate) )
        cerr << Error("CJwave failed") << Exit;
    state->V  = wstate.V;
    state->e  = wstate.e;
    state->u  = wstate.u;
    state->us = wstate.us;
    int index = Xeos.Zref()->var_index("lambda");
    if( index >= 0 )
        (state->z)[index] = 1.0;
    delete det;
    state->Evaluate();
    cout << "CJ state\n" << *state;
       
    return 0;
}
