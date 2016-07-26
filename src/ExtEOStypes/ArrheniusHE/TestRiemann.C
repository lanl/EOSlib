#include <Arg.h>
#include <LocalMath.h>
#include "ArrheniusHE.h"
#include "Riemann_genX.h"
//
int main(int, char **argv)
{
    ProgName(*argv);
    // const char *file = "Test.data";
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ArrheniusTest.data";
    const char * file = file_.c_str();
    
    const char *type = "ArrheniusHE";
    const char *name = "HMX";
    const char *units = NULL;

    double V1 = 0.526;
    double e1 = 0.1717;
    double u1 = 0.1507;
    double lambda1 = 0.5;
    double V2 = 0.5254;
    double e2 = 0.1749;
    double u2 = 0.003114;
    double lambda2 = 0.0;
    V1 = 0.432248632;
    e1 = 4.50436885;
    u1 = 1.48333336;
    lambda1 = 0.03048;
    V2 = 0.490462722;
    e2 = 0.811045211;
    u2 = 0.363112572;
    lambda2 = 1.219e-14;
//
    V1 = 0.312817701;
    e1 = 7.80249609;
    u1 = 3.83638554;
    lambda1 =  0.0625642321;
    V2 = 0.457286486;
    e2 = 2.67495255;
    u2 = 0.889635574;
    lambda2 = 0.000891753179;


    double P = 5.5;

    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        GetVar(P,P);
        //
        GetVar(V1,V1);
        GetVar(e1,e1);
        GetVar(u1,u1);
        GetVar(lambda1,lambda1);
        //
        GetVar(V2,V2);
        GetVar(e2,e2);
        GetVar(u2,u2);
        GetVar(lambda2,lambda2);
        //
        ArgError;
    }
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
    ExtEOS *xeos = dynamic_cast<ExtEOS*>(eos);
    if( xeos == NULL )
        cerr << Error("xeos is NULL") << Exit;
    //
    HydroState hydro1(V1,e1,u1);
    double z1[1] = {lambda1};    
    HydroState hydro2(V2,e2,u2);
    double z2[1] = {lambda2};

    ExtEOS &eos1 = *xeos->Copy(z1);
    ExtEOS &eos2 = *xeos->Copy(z2);

    WaveState wave1, wave2;
    eos1.Evaluate(hydro1,wave1);
    eos2.Evaluate(hydro2,wave2);

    cout << " left state " << eos1.Type()
           << "::" << eos1.Name() << "\n";
    WaveStateLabel(cout << "\t") << "\n";
    cout << "\t" << wave1 << "\n";
    eos1.Zref()->Print(cout,z1);

    cout << "right state " << eos2.Type()
           << "::" << eos2.Name() << "\n";
    WaveStateLabel(cout << "\t") << "\n";
    cout << "\t" << wave2 << "\n";
    eos2.Zref()->Print(cout,z2);
    //    
    /*****************
    Isentrope *wc1 = eos1.isentrope(hydro1);
    if( wc1 == NULL )
        cerr << Error("wc1 is NULL") << Exit;
    if( wc1->P(P,LEFT,wave1) )
        cerr << Error("wc1->P failed") << Exit;
    cout << " left wave\n";
    WaveStateLabel(cout << "\t") << "\n";
    cout << "\t" << wave1 << "\n";

    Hugoniot *wc2 = eos2.shock(hydro2);
    if( wc2 == NULL )
        cerr << Error("wc2 is NULL") << Exit;
    if( wc2->P(wave1.P,RIGHT,wave2) )
        cerr << Error("wc2->P failed") << Exit;
    cout << "right wave\n";
    WaveStateLabel(cout << "\t") << "\n";
    cout << "\t" << wave2 << "\n";
    ******************/
    //
    RiemannSolver_genX RS(eos1,z1, eos2,z2);
    if( RS.Solve(hydro1,hydro2,wave1,wave2) )
        cerr << Error("ExactRiemann failed") << Exit;
    //
    cout << " left wave\n";
    WaveStateLabel(cout << "\t") << "\n";
    cout << "\t" << wave1 << "\n";
    cout << "right wave\n";
    WaveStateLabel(cout << "\t") << "\n";
    cout << "\t" << wave2 << "\n";
    //
    deleteEOS(eos);
    return 0;
}
