#include <Arg.h>
#include <String.h>

#include "ExtEOSmanager.h"


int main(int, char **argv)
{
	ProgName(*argv);
    EOS::Init();
    //
    const char *material = NULL;
    const char *uname    = "std";
    const char *state    = NULL;
	const char *file     = "HE.data";
    const char *lib      = "../lib/Linux";
    const char *EOSlog   = "EOSlog";
    // parameters for debugging
    material = "HEburn::PBX9501.BM.mod";
    state    = "V=0.376695637;"
               "e=8.91044016;"
               "lambda=0.1570E-04";
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
        GetVar(material,material);
        GetVar(uname,uname);
        GetVar(lib,lib);
        GetVar(EOSlog,EOSlog);
        //
        GetVar(state,state);
        //
		ArgError;
	}
    ExtEOSmanager *pt_Xmanager = NULL;
    ofstream *EOS_log = NULL;
    if( EOSlog )
    {
        EOS_log = new ofstream(EOSlog);
        pt_Xmanager = new ExtEOSmanager(*EOS_log);
    }
    else
    {
        pt_Xmanager = new ExtEOSmanager(cout);
    }
    ExtEOSmanager &Xmanager = *pt_Xmanager;
    //
    char *SharedLibDir = Cat( "SharedLibDirEOS=",lib );
    Xmanager.SetEnv(SharedLibDir);
    delete [] SharedLibDir;
    if( Xmanager.Read(file) )
        cerr << Error("error reading file ") << file << Exit;
    //
    //Xmanager.List(cout, "Units:hydro::*");
    //Xmanager.List(cout, "EOS:HEburn::*");
    //
    int n_mat = Xmanager.index_eos(material,uname);
    if( n_mat < 0 )
        cerr << Error("Could not find or initialize material\n")
             << Exit;
    ExtEOS &e = Xmanager.xeos(n_mat);
    ExtEOSstate *s0 = new ExtEOSstate(e);
    int n0 = Xmanager.AddState("state0",s0);    // s0 set to NULL
    if( state == NULL )
    {
        cout << Xmanager.state(n0) << "0\n";
    }
    else
    {
        ExtEOSstate *s1 = new ExtEOSstate(Xmanager.state(n0));   
        Calc calc;
        s1->Load(calc);
        double ans;
        SplitString Lines(state);
        const char *line;
        while( (line=Lines.WordTo(";")) )
        {
            if( calc.parse(line,ans) )
                cerr << Error("calc parse error\n") << Exit;
        }
        s1->Evaluate();
        if( e.NotInDomain(s1->V,s1->e,s1->z) )
            cerr << Error("NotInDomain") << Exit;
        int n1 = Xmanager.AddState("state1",s1);    // s1 set to NULL
        cout << Xmanager.state(n1) << "0\n";            
    }
    delete pt_Xmanager;
    return 0;
}
