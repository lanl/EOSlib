#include <Arg.h>
#include <LocalMath.h>
#include "HEburn.h"
#include "HotSpotCC_Rate.h"
//
#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    EOS::Init();
    InitFormat();

    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/HEburnTest.data";
    const char * file = file_.c_str();

    //const char *file = "HE.data";
    const char *type = "HEburn";
    const char *name = "PBX9502.HotSpotCC";
    const char *units = "hydro::std";

    double dt1  = 0.001;
    double dt2  = 0.010;
    double tmax = 0.5;


    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        
        ArgError;
    }
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);

    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;

    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    HEburn *HE = dynamic_cast<HEburn *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed for HEburn") << Exit;
    PTequilibrium *PT = HE->HE;
    EOS *reactants = HE->Reactants();
    EOS *products  = HE->Products();
    const Units *u = eos->UseUnits();

    HydroState state0(eos->V_ref,eos->e_ref);
    WaveState wstate0;
    if( eos->Evaluate(state0,wstate0) )
        cerr << Error("eos->Evaluate failed") << Exit;
    double P0 = wstate0.P;
    Detonation *det = HE->detonation(state0,P0);
    WaveState CJstate;
    if( det->CJwave(RIGHT, CJstate) )
        cerr << Error("CJstate failed\n") << Exit;
    Hugoniot *hug = reactants->shock(state0);
    WaveState VNstate;
    if( hug->u_s(CJstate.us,RIGHT,VNstate) )
        cerr << Error("CJstate failed\n") << Exit;

    cout << "       "; WaveStateLabel(cout) << "\n";
    cout << "       "; WaveStateLabel(cout, *u) << "\n";
    cout << "state0 "; cout << wstate0 << "\n";
    cout << "VN     "; cout << VNstate << "\n";
    cout << "CJ     "; cout << CJstate << "\n";

    HErate *Rate = HE->rate;
   


    HotSpotCC_Rate *RateCC =  dynamic_cast<HotSpotCC_Rate *>(Rate);
    if( RateCC == NULL )
        cerr << Error("dynamic_cast failed for HotSpotCC_Rate") << Exit;
    IDOF *idof = RateCC->Idof();
    double *z = &(*idof);
    z[0] = VNstate.P;

/** Test SetVerbose
eos->SetVerbose(1);
cout << "DEBUG: Rate->verbose " << Rate->verbose << "\n";
z[1] = 1.;
z[3] = 5;
z[4] = 0.07;
RateCC->TimeStep(VNstate.V,VNstate.e,z,dt2);
exit(1);
**/


    int i;
    cout.setf(ios::left, ios::adjustfield);
    cout << setw(12) << "  t" << " "
         << setw(12) << "  lambda" << " "
         << setw(12) << "  s2"     << " "
         << setw(12) << "  ecc"    << "\n";
    cout.setf(ios::right, ios::adjustfield);
    double t = 0.;
    while( z[2] < 0.999 )
    {
        z[1] = t;
        if( RateCC->Integrate(VNstate.V,VNstate.e,z,dt1) )
            cerr << Error("RateCC->Integrate failed") << Exit;
        cout << setw(12) << setprecision(6) << z[1] << " "
             << setw(12) << setprecision(6) << z[2] << " "
             << setw(12) << setprecision(6) << z[4] << " "
             << setw(12) << setprecision(6) << z[5] << "\n";
        t += dt1;
    }
    z[2] = 1.0;
    while( t<tmax )
    {
        z[1] = t;
        double dt;
        int source = RateCC->TimeStep(VNstate.V,VNstate.e,z,dt);
        if( source != 3 || dt <= 0.0 )
            cout << "source " << source << "  dt " << dt << "\n";
        if( RateCC->Integrate(VNstate.V,VNstate.e,z,dt2) )
            cerr << Error("RateCC->Integrate failed") << Exit;
        cout << setw(12) << setprecision(6) << z[1] << " "
             << setw(12) << "1.0         "          << " "
             << setw(12) << setprecision(6) << z[4] << " "
             << setw(12) << setprecision(6) << z[5] << "\n";
        t += dt2;
    }


// Cleanup
    delete idof;
    delete hug;
    delete det;
    deleteEOS(reactants);
    deleteEOS(products);
    deleteEOS(eos);
    return 0;
}
