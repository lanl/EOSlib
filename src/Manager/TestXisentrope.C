#include <Arg.h>
#include <String1.h>

#include "ExtEOSmanager.h"
#include "Riemann_genX.h"
#include "Isentrope_ODE.h"

void Evaluate(ExtEOSstate &s, const char *state)
{
    Calc calc;
    s.Load(calc);
    SplitString Lines(state);
    const char *line;
    double ans;
    while( (line=Lines.WordTo(";")) )
    {
        if( calc.parse(line,ans) )
            cerr << Error("calc parse error on line ")
                 << line << "\n" << Exit;
    }
    s.Evaluate();
}

const char *help[] = {    // list of commands printed out by help option
    "material    name    # type::name",
    "file[s]     file    # colon separated list of data files",
    "lib         name    # directory for EOSlib shared libraries",
    "                    # default environ variable EOSLIB_SHARED_LIBRARY_PATH",
    "mat         name    # type::name",
    "uname       name    # units",
    "state       string  # 'V=NUM; e=NUM; . . .'",
    "count       int     # number of integration steps for isentrope ODE",
    "P           num     # point on isentrope with specified pressure",
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
    const char *files  = NULL;
    const char *lib    = NULL;

    const char *mat    = NULL;
    const char *uname  = "std";
    const char *state  = NULL;

    const char *EOSlog = "EOSlog";

    int count = 10;
    double P = -1.;
    while(*++argv)
    {
        GetVar(file,files);
        GetVar(files,files);
        GetVar(lib,lib);

        GetVar(mat,mat);
        GetVar(uname,uname);
        GetVar(EOSlog,EOSlog);
        //
        GetVar(state,state);
        GetVar(P,P);
        //
        GetVar(count,count);

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
    int iunits = Xmanager.index_units(uname);
    if( iunits < 0 )
        cerr << Error("Failed fetching units ")
             << uname << "\n" << Exit;
    Units &U = Xmanager.units(iunits);    
    //
    int n = Xmanager.index_eos(mat,uname);
    if( n < 0 )
        cerr << Error("Could not find or initialize material ")
             << mat << "\n" << Exit;
    ExtEOS &eos = Xmanager.xeos(n);
    ExtEOSstate *s = new ExtEOSstate(eos);   
    Evaluate(*s,state);
    HydroState hstate(s->V,s->e,s->u);
    //
    /*****************
    ExtEOS *eos1 = eos.Copy(s->z);
    Isentrope_ODE I(*eos1);
    deleteExtEOS(eos1);
    if( I.Initialize(hstate) )
        cerr << Error("initialize failed") << Exit;
    *****************/
    /*************/
    Isentrope *I1 = eos.isentrope(hstate,s->z);
    if( I1 == NULL )
        cerr << Error("I1 is NULL") << Exit;    
    Isentrope_ODE *Iode = dynamic_cast<Isentrope_ODE *>(I1);
    if( Iode == NULL )
        cerr << Error("isentrope not ODE") << Exit;
    Isentrope_ODE &I = *Iode;
    /*************/
    //
    double V;
    double y[2],yp[2];
    I.LastState(V,y,yp);    
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(13) << "   V" << " "
         << setw(13) << "   e" << " "
         << setw(13) << "   u" << " "
         << setw(13) << "   p" << " "
         << setw(13) << "   T" << " "
         << setw(13) << "   c" << "\n";
	cout.setf(ios::right, ios::adjustfield);
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
    double T = eos.T(s->V,s->e,s->z);
    double c = eos.c(s->V,s->e,s->z);
    cout << s->V << " " << setw(13) << setprecision(6) << s->e
                 << " " << setw(13) << setprecision(6) << s->u
                 << " " << setw(13) << setprecision(6) << s->P
                 << " " << setw(13) << setprecision(6) << T
                 << " " << setw(13) << setprecision(6) << c
                 << "\n";
    while( count-- )
    {
        if( I.Forward(V,y,yp) )
            break;
        T = eos.T(V,y[0],s->z);
        c = eos.c(V,y[0],s->z);
        cout << V << " " << setw(13) << setprecision(6) << y[0]
                  << " " << setw(13) << setprecision(6) << y[1]
                  << " " << setw(13) << setprecision(6) << -yp[0]
                  << " " << setw(13) << setprecision(6) << T
                  << " " << setw(13) << setprecision(6) << c
                  << "\n";
    }
    if( P > 0 )
    {
        WaveState wave;
        int status = I.P(P,LEFT,wave);
        if( status )
            cerr << Error("I.P failed") << Exit;
        T = eos.T(wave.V,wave.e,s->z);
        c = eos.c(wave.V,wave.e,s->z);
        cout << wave.V << " " << setw(13) << setprecision(6) << wave.e
                       << " " << setw(13) << setprecision(6) << wave.u
                       << " " << setw(13) << setprecision(6) << wave.P
                       << " " << setw(13) << setprecision(6) << T
                       << " " << setw(13) << setprecision(6) << c
                       << "\n";
    }
    delete s;
    return 0;
}
