#include <Arg.h>
#include <LocalMath.h>
#include "HEburn2.h"
#include "HotSpotRateCC.h"
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
 
    //const char *file = "Test.data";
    const char *type = "HEburn2";
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
    HEburn2 *HE = dynamic_cast<HEburn2 *>(eos);
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

    //HErate2 *Rate = HE->rate;
    //HotSpotRateCC *RateCC =  dynamic_cast<HotSpotRateCC *>(Rate);
    //if( RateCC == NULL )
    //    cerr << Error("dynamic_cast failed for HotSpotCC_Rate") << Exit;
    //IDOF *idof = RateCC->Idof();
    IDOF *idof = HE->Zref()->Copy();
    double *z = &(*idof);
    z[0] = VNstate.P;
    z[1] = 0.0;
    double V = VNstate.V;
    double e = VNstate.e;
    cout << "\nBurn at VN state (V,e)\n";

    cout.setf(ios::left, ios::adjustfield);
    cout << setw(6)  << "  t"      << "  "
         << setw(8)  << " dt"      << "  "
         << setw(10) << " lambda"   << "  "
         << setw(10) << " lambda2" << "  "
         << setw(8)  << " ecc"     << "  "
         << setw(5)  << " P"       << "  "
         << setw(5)  << " T"       << "\n";
    cout.setf(ios::right, ios::adjustfield);
    double t = 0.;
    double dt;
    int source;
    double lambda, lambda2;
    double P, T, ecc;
    HE->get_lambda(z, lambda,lambda2);
    while( lambda < 0.999 )
    {
        source = HE->TimeStep(VNstate.V,VNstate.e,z,dt);
        if( source != 3 || dt <= 0.0 )
            cout << "source " << source << "  dt " << dt << "\n";
        if( HE->Integrate(VNstate.V,VNstate.e,z,dt1) )
            cerr << Error("HE->Integrate failed") << Exit;
        t += dt1;
        HE->get_lambda(z,lambda,lambda2);
        P = HE->P(V,e,z);
        T = HE->T(V,e,z);
        (void) HE->var("e_cc",V,e,z,ecc);
        cout << setw(6)  << setprecision(3) << fixed      << t   << "  "
             << setw(8)  << setprecision(2) << scientific << dt  << "  "
             << setw(10) << setprecision(4) << lambda            << "  "
             << setw(10) << setprecision(4) << lambda2           << "  "
             << setw(8)  << setprecision(2) << ecc               << "  "
             << setw(5)  << setprecision(2) << fixed << P        << "  "
             << setw(5)  << setprecision(0) << T                 << "\n";
    }
    lambda =1.0;
    HE->set_lambda(lambda,lambda2, z);
    while( t<tmax )
    {
        source = HE->TimeStep(VNstate.V,VNstate.e,z,dt);
        if( source != 3 || dt <= 0.0 )
            cout << "source " << source << "  dt " << dt << "\n";
        if( HE->Integrate(VNstate.V,VNstate.e,z,dt2) )
            cerr << Error("HE->Integrate failed") << Exit;
        t += dt2;
        HE->get_lambda(z, lambda,lambda2);
        P = HE->P(V,e,z);
        T = HE->T(V,e,z);
        (void) HE->var("e_cc",V,e,z,ecc);
        cout << setw(6)  << setprecision(3) << fixed      << t   << "  "
             << setw(8)  << setprecision(2) << scientific << dt  << "  "
             << setw(10) << left            << "1.0"  << right   << "  "
             << setw(10) << setprecision(4) << lambda2           << "  "
             << setw(8)  << setprecision(2) << ecc               << "  "
             << setw(5)  << setprecision(2) << fixed << P        << "  "
             << setw(5)  << setprecision(0) << T                 << "\n";
    }
    P = HE->HE->eos2->P(V,e);
    T = HE->HE->eos2->T(V,e);
    double GammaV = HE->HE->eos2->Gamma(V,e)/V;
    cout << "products at VN state:"
         << "  P " << setprecision(3) << P
         << "  T " << setprecision(1) << T
         << "  Gamma/V " << setprecision(3) << GammaV
         << "\n";
// Cleanup
    delete idof;
    delete hug;
    delete det;
    deleteEOS(reactants);
    deleteEOS(products);
    deleteEOS(eos);
    return 0;
}
