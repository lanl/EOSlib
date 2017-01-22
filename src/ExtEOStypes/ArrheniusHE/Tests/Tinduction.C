#include <Arg.h>
#include <LocalMath.h>
#include "ArrheniusHE.h"
//
#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    //const char *file = "Test.data";
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ArrheniusTest.data";
    const char * file = file_.c_str();
    
    const char *type = "ArrheniusHE";
    const char *name = "HMX";
    const char *units = NULL;

    EOS::Init();
    double Pmin = 10;
    double Pmax = 65;
    int nsteps = 11;
    double dP = NaN;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        GetVar(Pmin,Pmin);
        GetVar(Pmax,Pmax);
        GetVar(dP,dP);
        GetVar(nsteps,nsteps);
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
    ArrheniusHE *HE = dynamic_cast<ArrheniusHE *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    EOS *reactants = HE->reactants();
    EOS *products  = HE->products();
    double lambda_ref = HE->lambda_ref();
    double z[1] = {lambda_ref};
    //
    double Vref = eos->V_ref; 
    double eref = eos->e_ref;
    double Pref = HE->P(Vref,eref);
    double Tref = HE->T(Vref,eref);
    WaveState CJ;
    HydroState state0(Vref,eref);
    Detonation *det = HE->detonation(state0,Pref);
    if( det == NULL )
        cerr << Error("HE->detonation failed" ) << Exit;
    if( det->CJwave(RIGHT,CJ) )
        cerr << Error("det->CJwave failed" ) << Exit;
    double Dcj = CJ.us;
    double Pcj = CJ.P;
    double Tcj = products->T(CJ.V, CJ.e);
    cout << "Dcj, Pcj, Tcj = " << Dcj << ", " << Pcj << ", " << Tcj << "\n";
    Hugoniot *hug = reactants->shock(state0);
    if( hug == NULL )
        cerr << Error("eos1->shock failed") << Exit;
    WaveState VN;
    if( hug->u_s(Dcj,RIGHT,VN) )
        cerr << Error("hug->u_s failed") << Exit;
    double Pvn = VN.P; 
    cout << "Pvn = " << Pvn << "\n";
    //
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(4) << " P"
         << " " << setw(5) << "  T"
         << " " << setw(8) << "t_induct"
         << " " << setw(5) << "Tpcj"
         << " " << setw(8) << "t_induct"
         << "\n";
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::showpoint);
    int i;
    double P;
    if( std::isnan(dP) )
        dP = (Pmax-Pmin)/nsteps;
    else
        nsteps = int((Pmax-Pmin)/dP + 0.99);
    for( P=Pmin, i=0; i<=nsteps; i++, P+=dP )
    {
        WaveState shock;
        if( hug->P(P,RIGHT,shock) )
            cerr << Error("hug->P failed") << Exit;      
        double T = reactants->T(shock.V,shock.e);
        double t1 = HE->Dt(shock.V,shock.e,z,0.9);
	    cout.setf(ios::fixed, ios::floatfield);
        cout <<        setw(4) << setprecision(1) << P
             << " " << setw(5) << setprecision(0) << T;
	    cout.setf(ios::scientific, ios::floatfield);
        cout << " " << setw(8) << setprecision(2) << t1;
        if( P > Pvn )
        {
            cout << "\n";
            break;
        }
        if( P > Pcj )
        {
            cout << "\n";
            continue;
        }
        // second shock to Pcj
        Hugoniot *hug2 = reactants->shock(shock);
        if( hug2 == NULL )
            cerr << Error("eos1->shock failed") << Exit;
        if( hug2->P(Pcj,RIGHT,shock) )
            cerr << Error("hug->P failed") << Exit;
        T = reactants->T(shock.V,shock.e);
        t1 = HE->Dt(shock.V,shock.e,z,0.9);
	    cout.setf(ios::fixed, ios::floatfield);
        cout << " " << setw(5) << setprecision(0) << T;
	    cout.setf(ios::scientific, ios::floatfield);
        cout << " " << setw(8) << setprecision(2) << t1
             << "\n";
        delete hug2;
    }  
    deleteEOS(products);
    deleteEOS(reactants);
    deleteEOS(eos);
    return 0;
}
