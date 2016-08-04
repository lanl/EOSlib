#include <Arg.h>
#include <EOS.h>

using namespace std;

int main(int, char **argv)
{
    ProgName(*argv);
    EOS::Init();
    InitFormat();

    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ApplicationsEOS.data";
    const char * files = file_.c_str();
    std::string libPath;
    libPath  = (getenv("EOSLIB_SHARED_LIBRARY_PATH") != NULL) ? getenv("EOSLIB_SHARED_LIBRARY_PATH") : "PATH ENV NOT SET!";
    const char * lib     = libPath.c_str();
    //const char *files    = "EOS.data";
    //const char *lib      = "../lib/Linux";
    const char *type     = NULL;
    const char *name     = NULL;
    const char *material = "BirchMurnaghan::HMX";//NULL;
    const char *units    = "hydro::std";

    double P = 1e-4;
    double T1 = 100.;
    double T2 = 10000.;
    int nsteps = 10;

// process command line arguments
    while(*++argv)
    {
        GetVar(file,files);
        GetVar(files,files);
        GetVar(lib,lib);
        GetVar(type,type);
        GetVar(name,name);
        GetVar(material,material);
        GetVar(units,units);

        GetVar(P,P);
        GetVar(T1,T1);
        GetVar(T2,T2);
        GetVar(nsteps,nsteps);
        ArgError;
    }
#ifdef LINUX
    setenv("SharedLibDirEOS",lib,1);
#else
    putenv(Cat("SharedLibDirEOS=",lib));
#endif
    // input check
    if( material )
    {
        if( type || name )
            cerr << Error("Can not specify both material and name or type")
                 << Exit;
        if( TypeName(material,type,name) )
            cerr << Error("syntax error, material = ") << material
                 << Exit;
    }
    else if( type==NULL || name==NULL )
    {
        cerr << Error("Specify either material or name and type")
             << Exit;
    }
// fetch eos
    DataBase db;
    if( db.Read(files) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("material not found, ") << type << "::" << name << Exit;    
    if( eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;

    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
    cout.setf(ios::left, ios::adjustfield);

    double Tfact = exp(log(T2/T1)/double(nsteps));
    int i;
    double T = T1;
    HydroState stateT;
    HydroThermal HTstate;

    HydroThermalLabel(cout) << " "
      << setw(12) <<  "    gamma"
      << "\n";
    const Units *u = eos->UseUnits();
    if( u )
    {
        HydroThermalLabel(cout, *u) << " "
          << setw(12) <<  "     ---"
          << "\n";
    }
    cout.setf(ios::right, ios::adjustfield);
    for(i=0; i<=nsteps; i++, T *= Tfact )
    {
        if( eos->PT(P,T,stateT) )
           cerr << Error("eos->PT failed") << Exit;
        eos->Evaluate(stateT, HTstate);
        HTstate.u = 0.0;
        double gamma = HTstate.c*HTstate.c/(HTstate.P*HTstate.V);
        cout << HTstate
             << " " << setw(12) << gamma
             << "\n";
    }
    deleteEOS(eos);
    return 0;
}
