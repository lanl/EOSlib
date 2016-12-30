#include <Arg.h>
#include <String1.h>

#include "ExtEOSmanager.h"


const char *help[] = {    // list of commands printed out by help option
    "material    name    # type::name",
    "file[s]     file    # colon separated list of data files",
    "lib         name    # directory for EOSlib shared libraries",
    "                    # default environ variable EOSLIB_SHARED_LIBRARY_PATH",
    "uname       name    # units",
    "",
    "material    name    # type::name",
    "state       string  # 'V=NUM; e=NUM; . . .'",
    "EOSlog      file    # name of file with eos errors",
    0
};

void Help(int status)
{
    const char **list;
    for(list=help ;*list; list++)
    {
        cerr << *list << "\n";
    }
    exit(status);
}

int main(int, char **argv)
{
    ProgName(*argv);
    EOS::Init();
    //
    const char *files    = NULL;
    const char *lib      = NULL;

    const char *material = NULL;
    const char *uname    = "std";
    const char *state    = NULL;

    const char *EOSlog   = "EOSlog";
	while(*++argv)
	{
		GetVar(file,files);
		GetVar(files,files);
        GetVar(lib,lib);

        GetVar(material,material);
        GetVar(uname,uname);
        GetVar(EOSlog,EOSlog);
        //
        GetVar(state,state);
        //
        if( !strcmp(*argv, "?") || !strcmp(*argv,"help") )
            Help(0);
		ArgError;
	}
    if( files==NULL )
        cerr << Error("must specify data file") << Exit;    
    if( lib==NULL && !(lib=getenv("EOSLIB_SHARED_LIBRARY_PATH")) )
    {
        cerr << Error("must specify lib or export EOSLIB_SHARED_LIBRARY_PATH")
             << Exit;  
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
    string lib_str("EOSLIB_SHARED_LIBRARY_PATH=");
    lib_str += lib;
    Xmanager.SetEnv(lib_str.c_str());
    //
    if( Xmanager.Read(files) )
        cerr << Error("error reading file(s) ") << files
             << ", see EOSlog" << Exit;
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
