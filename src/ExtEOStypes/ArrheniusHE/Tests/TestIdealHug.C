#include <Arg.h>
#include <LocalMath.h>
#include "IdealHE.h"
//
#define NaN EOS::NaN

int HugFail(double P0, HydroState &state0, WaveState &shock1)
{
    double us = shock1.us;
    double P1 = shock1.P;
    double m1  = (us-shock1.u)/shock1.V;
    double m0  = (us-state0.u)/state0.V;
    if( std::abs(m1-m0) > 0.001*std::abs(m1) + 1.e-6 )
        return 1;
    double p0  = P0 + m0*m0*state0.V;
    double p1  = P1 + m1*m1*shock1.V;
    if( std::abs(p1-p0) > 0.001*std::abs(p1) + 1.e-6 )
        return 1;
    double h0 = 0.5*sqr(m0*state0.V) + state0.e + P0*state0.V;
    double h1 = 0.5*sqr(m1*shock1.V) + shock1.e + P1*shock1.V;
    if( std::abs(h1-h0) > 0.001*std::abs(h1) + 1.e-6 )
        return 1;
    return 0;    
}

int diff(WaveState &shock1, WaveState &shock2)
{
    if( std::abs(shock1.V - shock2.V) > 0.001*shock1.V + 1e-6)
        return 1;
    if( std::abs(shock1.e - shock2.e) > 0.001*std::abs(shock1.e) + 1e-6)
        return 1;
    if( std::abs(shock1.u - shock2.u) > 0.001*std::abs(shock1.u) + 1e-6 )
        return 1;
    if( std::abs(shock1.P - shock2.P) > 0.001*shock1.P + 1e-6)
        return 1;
    if( std::abs(shock1.us - shock2.us) > 0.001*std::abs(shock1.us) + 1e-6)
        return 1;
    return 0;    
}

int main(int, char **argv)
{
    ProgName(*argv);
    //const char *file = "Test.data";
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ArrheniusTest.data";
    const char * file = file_.c_str();
    
    const char *type = "IdealHE";
    const char *name = "HMX";
    const char *units = NULL;

    EOS::Init();
    double V = NaN;
    double e = NaN;
    double lambda = 0.0;
    int nsteps = 10;

    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        GetVar(V,V);
        GetVar(e,e);
        GetVar(lambda,lambda);
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
    IdealHE *HE = dynamic_cast<IdealHE *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    (*HE->Zref())[0] = lambda;

    double Vref = eos->V_ref; 
    double eref = eos->e_ref;
    double lambda_ref = HE->lambda_ref();
    HydroState state0(Vref,eref);
    double Pref = HE->P(Vref,eref);

    EOS *products = HE->products();
    cout << "\nCJ detonation state\n";
    Detonation *det = HE->detonation(state0,Pref);
    if( det == NULL )
        cerr << Error("HE->detonation failed" ) << Exit;
    WaveState CJ;
    if( det->CJwave(RIGHT,CJ) )
        cerr << Error("det->CJwave failed" ) << Exit;
    double T = products->T(CJ.V,CJ.e);
    cout << "V e P T = " << CJ.V << ", " << CJ.e 
                 << ", " << CJ.P << ", " << T << "\n";
    cout << "D, u, c = " << CJ.us << ", " << CJ.u
                 << ", " << products->c(CJ.V,CJ.e) << "\n";
    Hugoniot *hug = HE->shock(state0);
    if( hug == NULL )
        cerr << Error("eos1->shock failed") << Exit;
    double u;
    int i;
    WaveStateLabel(cout << "\nShock locus\n")
        << " " <<setw(6) << "T " << "\n";
    for( i=0; i<=nsteps; i++ )
    {
        double u = 2.*double(i)/double(nsteps)*CJ.u;
        WaveState shock;
        if( hug->u(u,RIGHT,shock) )
            cerr << Error("hug->u failed" ) << Exit;
        T = HE->T(shock.V,shock.e);
	    cout.setf(ios::scientific, ios::floatfield);
        cout << shock;
	    cout.setf(ios::fixed, ios::floatfield);
        cout << " " << setw(6) << setprecision(0) << T
             << "\n";
        if( HugFail(Pref,state0,shock) )
            cout << "\t Hugoniot not satisfied\n";
        
        WaveState shock2;
        if( hug->P(shock.P,RIGHT,shock2) )
            cerr << Error("hug->P failed" ) << Exit;
        if( diff(shock,shock2) )
            cout << "\t hug->P differs\n";
        //
        if( hug->u_s(shock.us,RIGHT,shock2) )
            cerr << Error("hug->u_s failed" ) << Exit;
        if( diff(shock,shock2) )
            cout << "\t hug->u_s differs\n";
        //
        if( hug->V(shock.V,RIGHT,shock2) )
            cerr << Error("hug->V failed" ) << Exit;
        if( diff(shock,shock2) )
            cout << "\t hug->V differs\n";
    }
    WaveStateLabel(cout << "\nStrong detonation locus\n")
        << " " <<setw(6) << "T " << "\n";
    for( i=0; i<=nsteps; i++ )
    {
        double u = CJ.u + double(i)/double(nsteps)*CJ.u;
        WaveState shock;
        if( det->u(u,RIGHT,shock) )
            cerr << Error("det->u failed" ) << Exit;
        T = products->T(shock.V,shock.e);
	    cout.setf(ios::scientific, ios::floatfield);
        cout << shock;
	    cout.setf(ios::fixed, ios::floatfield);
        cout << " " << setw(6) << setprecision(0) << T
             << "\n";
        if( HugFail(Pref,state0,shock) )
            cout << "\t Hugoniot not satisfied\n";
        
        WaveState shock2;
        if( det->P(shock.P,RIGHT,shock2) )
            cerr << Error("det->P failed" ) << Exit;
        if( diff(shock,shock2) )
            cout << "\t det->P differs\n";
        //
        if( det->u_s(shock.us,RIGHT,shock2) )
            cerr << Error("det->u_s failed" ) << Exit;
        if( diff(shock,shock2) )
            cout << "\t det->u_s differs\n";
        //
        if( det->V(shock.V,RIGHT,shock2) )
            cerr << Error("det->V failed" ) << Exit;
        if( diff(shock,shock2) )
            cout << "\t det->V differs\n";        
    }
    cout << "\nCJ deflagration state\n";
    Deflagration *def = HE->deflagration(state0,Pref);
    if( def == NULL )
        cerr << Error("HE->deflagration failed" ) << Exit;
    if( def->CJwave(RIGHT,CJ) )
        cerr << Error("def->CJwave failed" ) << Exit;
    T = products->T(CJ.V,CJ.e);
    cout << setprecision(6)
         << "V e P T = " << CJ.V << ", " << CJ.e 
                 << ", " << CJ.P << ", " << T << "\n";
    cout << "D, u, c = " << CJ.us << ", " << CJ.u
                 << ", " << products->c(CJ.V,CJ.e) << "\n";
    
    WaveStateLabel(cout << "\nWeak deflagration locus\n")
        << " " <<setw(6) << "T " << "\n";
    for( i=nsteps; i>=0; i-- )
    {
        double u = double(i)/double(nsteps)*CJ.u;
        WaveState shock;
        if( def->u(u,RIGHT,shock) )
            cerr << Error("def->u failed" ) << Exit;
        T = products->T(shock.V,shock.e);
	    cout.setf(ios::scientific, ios::floatfield);
        cout << shock;
	    cout.setf(ios::fixed, ios::floatfield);
        cout << " " << setw(6) << setprecision(0) << T
             << "\n";
        if( HugFail(Pref,state0,shock) )
            cout << "\t Hugoniot not satisfied\n";
        
        WaveState shock2;
        if( def->P(shock.P,RIGHT,shock2) )
            cerr << Error("def->P failed" ) << Exit;
        if( diff(shock,shock2) )
        {
            cout << "\t def->P differs\n";
            cout << shock2 << "\n";
            return 1;
        }
        //
        if( def->u_s(shock.us,RIGHT,shock2) )
            cerr << Error("def->u_s failed" ) << Exit;
        if( diff(shock,shock2) )
        {
            cout << "\t def->u_s differs\n";
            cout << shock2 << "\n";
            return 1;
        }
        //
        if( def->V(shock.V,RIGHT,shock2) )
            cerr << Error("def->V failed" ) << Exit;
        if( diff(shock,shock2) )
        {
            cout << "\t def->V differs\n";        
            cout << shock2 << "\n";
            return 1;
        }
    }
    delete def;
    delete hug;
    delete det;
    deleteEOS(products);
    deleteEOS(eos);
    return 0;
}    
