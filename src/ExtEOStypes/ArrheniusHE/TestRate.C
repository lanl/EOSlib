#include <Arg.h>
#include <LocalMath.h>
#include "ArrheniusHE.h"
//
#define NaN EOS::NaN
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

    EOS::Init();
    double V = NaN;
    double e = NaN;
    double lambda = NaN;
    double Pj = NaN;

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
        GetVar(Pj,Pj);
        
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
    if( std::isnan(V) )
        V = HE->V_ref;            
    if( std::isnan(e) )
        e = HE->e_ref;
    if( std::isnan(lambda) )
        lambda = lambda_ref;
    else
        HE->set_lambda(lambda);
    //
    double Vref = eos->V_ref; 
    double eref = eos->e_ref;
    double P, T, S;
    double z[1], zdot[1];
    z[0] = 0.0;     // lambda = 0 (reactants)
    double Pref = HE->P(Vref,eref,z);
    double Tref = HE->T(Vref,eref,z);
    cout << "\nReference state: V, e = " << Vref << ", " << eref
         << "  lambda = " << lambda_ref << "\n";
    cout <<   "                 P, T = " << Pref << ", " << Tref << "\n";
    //
    double P0 = Pref;
    HydroState state0(Vref,eref);
    Hugoniot *shock = HE->shock(state0);
    if( shock == NULL )
        cerr << Error("HE->shock failed" ) << Exit;
    
	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(10) <<  " P"
         << " "  << setw(10) << "      T"
         << " "  << setw(10) << "    Tcbv"
         << " "  << setw(12) << "  t induct"
         << " "  << setw(12) << "  dT/dlambda"
         << "\n";
    P = P0;
    int nsteps = 20;
    double Pmax = 40;
    double dP = Pmax/double(nsteps);
    int i;
    for( i=0; i<=nsteps; i++ )
    {
        WaveState Pstate;
        P = (double(i)/double(nsteps))*Pmax;
        if( shock->P(P,RIGHT,Pstate) )
            cerr << Error("shcok->P failed" ) << Exit;
        T = HE->T(Pstate.V,Pstate.e);
        double Tcvb = products->T(Pstate.V,Pstate.e); // const vol burn 
        z[0] = lambda;
        double tinduct = HE->Dt(Pstate.V,Pstate.e,z,0.9);
        double dTdlambda, dPlambda;
        HE->ddlambda(Pstate.V,Pstate.e,lambda, dPlambda,dTdlambda);
        cout << setw(10) << setprecision(4) << P;
	    cout.setf(ios::right, ios::adjustfield);
        cout.setf(ios::fixed,ios::floatfield);
        cout << " "  << setw(10) << setprecision(0) << T
             << " "  << setw(10) << setprecision(0) << Tcvb;
	    //cout.setf(ios::left, ios::adjustfield);
        cout.setf(ios::scientific,ios::floatfield);
        cout << " "  << setw(12) << setprecision(4) << tinduct
             << " "  << setw(12) << setprecision(4) << dTdlambda
             << "\n";
        if( P <= Pj && Pj < P + dP )
        {
            // lambda, T, dTdlambda
	        cout.setf(ios::right, ios::adjustfield);
            cout << setw(6) << "lambda"
                 << " "  << setw(6) << "P   "
                 << " "  << setw(6) << "T "
                 << " "  << setw(10) << "dT/dlambda"
                 << " "  << setw(12) << "t_induct"
                 << " "  << setw(9) << "rate  "
                 << "\n";
            int j;
            for( j=0; j<=10; j++ )
            {
                double lambdaj = (j/10.);
                z[0] = lambdaj;
                P = HE->P(Pstate.V,Pstate.e,z);
                T = HE->T(Pstate.V,Pstate.e,z);
                HE->ddlambda(Pstate.V,Pstate.e,lambdaj, dPlambda,dTdlambda);
                tinduct = HE->Dt(Pstate.V,Pstate.e,z,0.9);
                double rate = HE->Rate(lambdaj,T);
	            cout.setf(ios::right, ios::adjustfield);
                cout.setf(ios::fixed,ios::floatfield);
                cout << setw(6) << setprecision(1) << lambdaj
                     << " "  << setw(6) << setprecision(2) << P
                     << " "  << setw(6) << setprecision(0) << T
                     << " "  << setw(10)<< setprecision(2) << dTdlambda;
                cout.setf(ios::scientific,ios::floatfield);
                cout << " "  << setw(12)<< setprecision(4) << tinduct
                     << " "  << setw(9) << setprecision(2) << rate
                     << "\n";
            }
        }
    }

    
/***************************    
    WaveState CJ;
    Detonation *det = HE->detonation(state0,P0);
    if( det == NULL )
        cerr << Error("HE->detonation failed" ) << Exit;
    if( det->CJwave(RIGHT,CJ) )
        cerr << Error("det->CJwave failed" ) << Exit;
    T = products->T(CJ.V,CJ.e);
    cout << "CJ state\n";
    cout << "V e P T = " << CJ.V << ", " << CJ.e 
                 << ", " << CJ.P << ", " << T << "\n";
    cout << "D, u, c = " << CJ.us << ", " << CJ.u
                 << ", " << products->c(CJ.V,CJ.e) << "\n";
    double t_induction = HE->t_induction(Vref,eref,lambda_ref);
    cout << "t induction = " << t_induction << "\n";
***************************/    
    //
    deleteEOS(products);
    deleteEOS(reactants);
    deleteEOS(eos);
    return 0;
}
