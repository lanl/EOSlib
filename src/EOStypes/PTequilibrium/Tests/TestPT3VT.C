#include <Arg.h>

#include <PTequilibrium_VT.h>
#include <EOS.h>
//
// need to set environment variable SharedLibDirEOS
// cd ../..; . SetEnv
//
#define NaN EOS_VT::NaN
int main(int, char **argv)
{
	ProgName(*argv);
	std::string file_;
	file_ = getenv("EOSLIB_DATA_PATH");
	file_ += "/test_data/PTequilibriumTest.data";
	const char *file = file_.c_str();
	const char *type = "PTequilibrium";
	const char *name = "mix0.8";
	const char *units = NULL;

    EOS_VT::Init();
    double Pmax = 50;
    double Pmin = 0.1;
    double nsteps = 10;
    double T = 500;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(Pmax,Pmax);
		GetVar(Pmin,Pmin);
		GetVar(T,T);
		GetVar(nsteps,nsteps);
		
		ArgError;
	}

	if( file == NULL )
		cerr << Error("Must specify data file") << Exit;
	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;
	EOS_VT *eos = FetchEOS_VT(type,name,db);
    if( eos == NULL )
    {
    	EOS *eos1 = FetchEOS(type,name,db);
    	if( eos1 == NULL )
    		cerr << Error("FetchEOS failed") << Exit;
    	eos = eos1->eosVT();
    	if( eos == NULL )
    		cerr << Error("Fetch failed") << Exit;
    }
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;

	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);
	cout.setf(ios::left, ios::adjustfield);
    //
    PTequilibrium_VT *eosPT = dynamic_cast<PTequilibrium_VT*>(eos);
    if( eosPT == NULL )
        cerr << Error("eosPT is NULL") << Exit;
    EOS *eos1 = eosPT->eos1->eosVe();
    if( eos1 == NULL )
        cerr << Error("eos1 is NULL") << Exit;
    EOS *eos2 = eosPT->eos2->eosVe();
    if( eos2 == NULL )
        cerr << Error("eos2 is NULL") << Exit;
    double V1_ref = eos1->V_ref;
    double e1_ref = eosPT->eos1->e(V1_ref,T);
    double P1_ref = eosPT->eos1->P(V1_ref,T);
    HydroState state1_ref(V1_ref,e1_ref);
    Isotherm *Teos1 = eos1->isotherm(state1_ref);
    if( Teos1 == NULL )
        cerr << Error("Teos1 is NULL") << Exit;     
    double V2_ref = eos2->V_ref;
    double e2_ref = eosPT->eos2->e(V2_ref,T);
    double P2_ref = eosPT->eos2->P(V2_ref,T);
    HydroState state2_ref(V2_ref,e2_ref);
    Isotherm *Teos2 = eos2->isotherm(state2_ref);
    if( Teos2 == NULL )
        cerr << Error("Teos2 is NULL") << Exit;

    double P0 = max(P1_ref,P2_ref);
    double dP = (Pmax-P0)/nsteps;
    int k;
    cout << setw(10) <<  " V"
         << " "  << setw(10) << "  P"
         << " "  << setw(10) << "  V1"
         << " "  << setw(10) << "  V2"
         << "   count\n";
    for( k=0; k<=nsteps; k++ )
    {
        double P = P0 + k*dP;
        ThermalState state1;
        ThermalState state2;
        if( Teos1->P(P,state1) )
            cerr << Error("Teos1 failed") << Exit;
        if( Teos2->P(P,state2) )
            cerr << Error("Teos2 failed") << Exit;
        double V = eosPT->lambda1*state1.V + eosPT->lambda2*state2.V;
        cout << setw(10) << setprecision(4) << V
             << " "  << setw(10) << setprecision(4) << P
             << " "  << setw(10) << setprecision(4) << state1.V
             << " "  << setw(10) << setprecision(4) << state2.V
             << "\n";
        
        P = eos->P(V,T);
        double V1 = eosPT->V1;
        double V2 = eosPT->V2;
        cout << setw(10) << " "
             << " "  << setw(10) << setprecision(4) << P
             << " "  << setw(10) << setprecision(4) << V1
             << " "  << setw(10) << setprecision(4) << V2
             << "   " << eosPT->count
             << "\n";
    }
    P0 = min(P1_ref,P2_ref);
    double P1 = max(P1_ref,P2_ref);
    dP = (P1-P0)/nsteps;
    cout << setw(10) <<  " V"
         << " "  << setw(10) << "  P"
         << " "  << setw(10) << "  V1"
         << " "  << setw(10) << "  V2"
         << "   count"
         << " "  << setw(10) << "  cT2_1"
         << " "  << setw(10) << "  cT2_2"
         << "\n";
    for( k=0; k<=nsteps; k++ )
    {
        double P = P1 - k*dP;
        ThermalState state1;
        ThermalState state2;
        if( Teos1->P(P,state1) )
            cerr << Error("Teos1 failed") << Exit;
        if( Teos2->P(P,state2) )
            cerr << Error("Teos2 failed") << Exit;
        double V = eosPT->lambda1*state1.V + eosPT->lambda2*state2.V;
        cout << setw(10) << setprecision(4) << V
             << " "  << setw(10) << setprecision(4) << P
             << " "  << setw(10) << setprecision(4) << state1.V
             << " "  << setw(10) << setprecision(4) << state2.V
             << "\n";
        
        P = eos->P(V,T);
        double V1 = eosPT->V1;
        double V2 = eosPT->V2;
        cout << setw(10) << " "
             << " "  << setw(10) << setprecision(4) << P
             << " "  << setw(10) << setprecision(4) << V1
             << " "  << setw(10) << setprecision(4) << V2
             << "   " << setw(5) << eosPT->count
             << " "  << setw(10) << setprecision(4) << eosPT->eos1->cT2(V1,T)
             << " "  << setw(10) << setprecision(4) << eosPT->eos2->cT2(V2,T)
             << "\n";
    }
    P0 = min(P1_ref,P2_ref);
    dP = (P0-Pmin)/nsteps;
    cout << setw(10) <<  " V"
         << " "  << setw(10) << "  P"
         << " "  << setw(10) << "  V1"
         << " "  << setw(10) << "  V2"
         << "   count"
         << " "  << setw(10) << "  cT2_1"
         << " "  << setw(10) << "  cT2_2"
         << "\n";
    for( k=0; k<=nsteps; k++ )
    {
        double P = P0 - k*dP;
        ThermalState state1;
        ThermalState state2;
        if( Teos1->P(P,state1) )
            cerr << Error("Teos1 failed") << Exit;
        if( Teos2->P(P,state2) )
            cerr << Error("Teos2 failed") << Exit;
        double V = eosPT->lambda1*state1.V + eosPT->lambda2*state2.V;
        cout << setw(10) << setprecision(4) << V
             << " "  << setw(10) << setprecision(4) << P
             << " "  << setw(10) << setprecision(4) << state1.V
             << " "  << setw(10) << setprecision(4) << state2.V
             << "\n";
        
        P = eos->P(V,T);
        double V1 = eosPT->V1;
        double V2 = eosPT->V2;
        cout << setw(10) << " "
             << " "  << setw(10) << setprecision(4) << P
             << " "  << setw(10) << setprecision(4) << V1
             << " "  << setw(10) << setprecision(4) << V2
             << "   " << setw(5) << eosPT->count
             << " "  << setw(10) << setprecision(4) << eosPT->eos1->cT2(V1,T)
             << " "  << setw(10) << setprecision(4) << eosPT->eos2->cT2(V2,T)
             << "\n";
    }

    deleteEOS_VT(eos);
    return 0;
}
