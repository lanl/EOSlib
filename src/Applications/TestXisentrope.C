#include <Arg.h>
#include <String.h>

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


int main(int, char **argv)
{
    ProgName(*argv);
    EOS::Init();
    //
    const char *uname = "std";
    //const char *file = "HE.data";
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ApplicationsHE.data";
    const char * file = file_.c_str();
    //const char *lib = "../lib/Linux";
    std::string libPath;
    libPath  = (getenv("EOSLIB_SHARED_LIBRARY_PATH") != NULL) ? getenv("EOSLIB_SHARED_LIBRARY_PATH") : "PATH ENV NOT SET!";
    const char * lib     = libPath.c_str();
    ofstream *EOS_log = NULL;
    ExtEOSmanager *pt_Xmanager = NULL;
    const char *EOSlog = "EOSlog";
    // Debugging input parameters
    const char *mat   = "HEburn::PBX9501.BM";
    //char *state = "V=4.51255657368965e-01; e=1.00457400813203e+01; u=1.44551758702307;"
    const char *state = "V=1.718828; e=8.385752; u=0;"
                        "lambda=1.09449320773967e-02";
    int count = 10;
    double P = -1.;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(uname,uname);
        GetVar(lib,lib);
        GetVar(EOSlog,EOSlog);
        //
        GetVar(mat,mat);
        GetVar(state,state);
        GetVar(P,P);
        //
        GetVar(count,count);
        ArgError;
    }
    EOS_log = new ofstream(EOSlog);
    ExtEOSmanager Xmanager(*EOS_log);
    char *SharedLibDir = Cat( "SharedLibDirEOS=",lib );
    Xmanager.SetEnv(SharedLibDir);
    delete [] SharedLibDir;
    if( Xmanager.Read(file) )
        cerr << Error("error reading file ") << file << Exit;
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
