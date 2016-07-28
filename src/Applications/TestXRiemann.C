#include <Arg.h>
#include <String.h>

#include <ExtEOSmanager.h>
#include "Riemann_genX.h"

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

void Header(const Units &u)
{
    int n = 10 + 6;
    n += V_form.width + e_form.width + u_form.width + P_form.width
        + us_form.width + c_form.width + T_form.width;
    int i;
    for(i=0; i<n;i++)
        cout << "-";
    cout << "\n";
    cout << setw(13) << "";
    WaveStateLabel(cout) << Center(c_form,"c") << " "
                         << Center(T_form,"T") << "\n";
    cout << setw(13) << "";
    WaveStateLabel(cout, u) << Center(u_form, u.Unit("velocity"))    << " "
                            << Center(T_form, u.Unit("temperature")) << " "
                            << "\n";
    for(i=0; i<n;i++)
        cout << "-";
    cout << "\n";
}

void PrintWave(const char* type, ExtEOS &eos, WaveState &wave,double *z)
{
    double c;
    double T;
    if( z )
    {
        c = eos.c(wave.V,wave.e,z);
        T = eos.T(wave.V,wave.e,z);    
    }
    else
    {
        c = eos.c(wave.V,wave.e);
        T = eos.T(wave.V,wave.e);    
    }
    cout.setf(ios::left,ios::adjustfield);
    cout << setw(10) << type;

    cout.setf(ios::right,ios::adjustfield);    
    cout << wave << " "
         << c_form << c << " "
         << T_form << T << "\n";
}

void PrintState(ExtEOS &eos, HydroState &state,double *z)
{
    WaveState wave;
    if( z == NULL )
        eos.Evaluate(state, wave);
    else
    {
        wave.V = state.V;
        wave.e = state.e;
        wave.u = state.u;
        wave.P  = eos.P(state.V,state.e,z);
        wave.us = eos.c(state.V,state.e,z);
    }
    PrintWave("", eos, wave,z);
    if( z )
        eos.Zref()->Print(cout,z);
}


int main(int, char **argv)
{
    ProgName(*argv);
    EOS::Init();
    //
    const char *uname  = "std";
    //const char *file   = "HE.data";
    //const char *lib    = "../lib/Linux";
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ApplicationsHE.data";
    const char * file = file_.c_str();
    std::string libPath;
    libPath  = (getenv("EOSLIB_SHARED_LIBRARY_PATH") != NULL) ? getenv("EOSLIB_SHARED_LIBRARY_PATH") : "PATH ENV NOT SET!";
    const char * lib     = libPath.c_str();

    ExtEOSmanager *pt_Xmanager = NULL;
    const char *EOSlog = "EOSlog";
    // parameters for debugging
    const char *mat1   = "Keane::TATB";
    const char *state1 = "V=4.580806519045897E-01;"
                         "e=5.324190201457957E-01;"
                         "u=1.994705557677465E+00;";
                       //"lambda=1.17374301793383e-02";
    const char *mat2   = "Keane::TATB";
    const char *state2 = "V=4.580806519045897E-01;"
                         "e=5.324190201457957E-01;"
                         "u=4.590453427252382E-01;";
                       //"lambda=4.85607647843223e-14";
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(uname,uname);
        GetVar(lib,lib);
        GetVar(EOSlog,EOSlog);
        //
        GetVar(mat1,mat1);
        GetVar(state1,state1);
        GetVar(mat2,mat2);
        GetVar(state2,state2);
        //
        ArgError;
    }
    ofstream *EOS_log = new ofstream(EOSlog);
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
    int n1 = Xmanager.index_eos(mat1,uname);
    if( n1 < 0 )
        cerr << Error("Could not find or initialize material ")
             << mat1 << "\n" << Exit;
    ExtEOS &eos1 = Xmanager.xeos(n1);
    //
    int n2 = Xmanager.index_eos(mat2,uname);
    if( n2 < 0 )
        cerr << Error("Could not find or initialize material ")
             << mat2 << "\n" << Exit;
    ExtEOS &eos2 = Xmanager.xeos(n2);
    //
    ExtEOSstate *s1 = new ExtEOSstate(eos1);   
    Evaluate(*s1,state1);
    //cout << *s1 << "\n";
    //
    ExtEOSstate *s2 = new ExtEOSstate(eos2);   
    Evaluate(*s2,state2);
    //cout << *s2 << "\n";
    //
    RiemannSolver_genX RS(eos1,s1->z, eos2,s2->z);
    HydroState hstate1(s1->V,s1->e,s1->u);
    HydroState hstate2(s2->V,s2->e,s2->u);
    //
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
    cout << "Material left: " << mat1 << "\n";
    PrintState(eos1,hstate1,s1->z);
    cout << "Material right: " << mat2 << "\n";
    PrintState(eos2,hstate2,s2->z);
    Header(U);
    //
    WaveState  wstate1;
    WaveState  wstate2;
    if( RS.Solve(hstate1,hstate2,wstate1,wstate2) )
        cerr << Error("Solve failed\n") << Exit;
    PrintWave( "left: ", eos1, wstate1,s1->z);
    PrintWave("right: ", eos2, wstate2,s2->z);
    //
    delete s1;
    delete s2;
    return 0;
}
