#include <Arg.h>

#include <EOS.h>
#include <Porous.h>


int main(int, char **argv)
{
    ProgName(*argv);
    //const char *file = "Test.data";
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/PorousTest.data";
    const char * file = file_.c_str();
    const char *type = "FrozenPorous";
    const char *name = "HMX.65f90";
    const char *units = "hydro::std";

    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(units,units);
            
        ArgError;
    }

    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;

    const char *info = db.BaseInfo("EOS");
    if( info == NULL )
		cerr << Error("No EOS info" ) << Exit;
    cout << "Base info\n"
         << info << "\n";

    info = db.TypeInfo("EOS",type);
    if( info == NULL )
		cerr << Error("No type info" ) << Exit;
    cout << "Type info\n"
         << info << "\n";
    eos->PrintAll(cout);

    cout << "Test PT\n";
    HydroState state0;
    double P0 = 0.1;
    double T0 = 400;
    if( eos->PT(P0,T0,state0) )
        cerr << Error("eos->PT failed\n") << Exit;
    HydroStateLabel(cout) << "\n";

    cout << state0 << "\n";
    cout << "P = " << eos->P(state0) << ", T = " << eos->T(state0) << "\n";

    state0.V = eos->V_ref;
    state0.e = eos->e_ref;
    state0.u = 0.0;
    P0 = eos->P(state0);
    cout << "P0 = " << P0 << "\n";
    WaveState wave;
    HydroState state1;
    double Ps = 3.14;

    cout << "\nTest Hugoniot\n";
    Hugoniot *shock = eos->shock(state0);
    if( shock == NULL )
        cerr << Error("eos.shock failed\n") << Exit;
    if( shock->P(Ps,RIGHT,wave) )
        cerr << Error("shock->P failed\n") << Exit;
    state1 = wave;
    cout << Ps << " = " << eos->P(state1) << "\n";
    double V0 = state0.V;
    double e_h  = state0.e + 0.5 *(wave.P + P0)*(V0 - wave.V);
    double up_h = state0.u + sqrt((wave.P - P0)*(V0 - wave.V));
    double us_h = V0*sqrt((wave.P - P0)/(V0 - wave.V));
    cout << wave.e << " = "   <<  e_h << "\n";
    cout << wave.u << " = "   << up_h << "\n";
    cout << wave.us << " = " << us_h << "\n";
    delete shock;
        
    cout << "\nTest Isentrope\n";
    Isentrope *isentrope = eos->isentrope(state0);
    if( isentrope == NULL )
        cerr << Error("eos.isentrope failed\n") << Exit;
    if( isentrope->P(Ps,RIGHT,wave) )
        cerr << Error("isentrope->P failed\n") << Exit;
    state1 = wave;
    cout << eos->S(state0) << " = " << eos->S(state1) << "\n";
    delete isentrope;
    deleteEOS(eos);
    
    return 0;
}
