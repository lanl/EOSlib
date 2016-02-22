#include <Arg.h>

#include <PTequilibrium.h>
#include <EOS_VT.h>
//
// need to set environment variable SharedLibDirEOS
// cd ../..; . SetEnv
//
#define NaN EOS::NaN
int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "PTequilibrium";
	const char *name = "mix0.8";
	const char *units = NULL;

    EOS_VT::Init();
    double Pmax = 50;
    double Pmin = 0.1;
    double nsteps = 10;
    double e = 1;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(Pmax,Pmax);
		GetVar(Pmin,Pmin);
		GetVar(e,e);
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
        cerr << Error("Fetch failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;

	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);
    //
    PTequilibrium *eosPT = dynamic_cast<PTequilibrium*>(eos);
    if( eosPT == NULL )
        cerr << Error("eosPT is NULL") << Exit;
    double lambda1 = eosPT->lambda1;
    EOS *eos1 = eosPT->eos1;
    double V1 = eos1->V_ref;
    double e1 = eos1->e_ref;
    double T1 = eos1->T(V1,e1);
    EOS *eos2 = eosPT->eos2;
    double lambda2 = eosPT->lambda2;
    double V2 = eos2->V_ref;
    double e2 = eos2->e_ref;
    double T2 = eos2->T(V2,e2);
    int count;
    for( count=1; count<11; count++ )
    {
        double dTde1 = 1./eos1->CV(V1,e1);
        double dTde2 = 1./eos2->CV(V2,e2);
        double de = e - lambda1*e1-lambda2*e2;
        double dTde_av = lambda1*dTde2 + lambda2*dTde1;
        e2 += (lambda1*(T1-T2)+dTde1*de)/dTde_av;
        e1 += (lambda2*(T2-T1)+dTde2*de)/dTde_av;
        T1 = eos1->T(V1,e1);
        T2 = eos2->T(V2,e2);
        if( isnan(T1) || T1 < 0 || isnan(T2) || T2 < 0 )
        {
            cerr << Error("Failed to equilibrate temperature") << Exit;
            return 1;
        }
        if( abs(T1-T2) < 1.e-4*(T1+T2) )
            break;
    }
    if( count==11 )
        cerr << Error("Failed to equilibrate temperature") << Exit;
    cout << "T1, T2 = " << T1 << ", " << T2 << "\n";
    double P1_ref = eos1->P(V1,e1);
    HydroState state1_ref(V1,e1);
    Isotherm *Teos1 = eos1->isotherm(state1_ref);
    if( Teos1 == NULL )
        cerr << Error("Teos1 is NULL") << Exit;     
    double P2_ref = eos2->P(V2,e2);
    HydroState state2_ref(V2,e2);
    Isotherm *Teos2 = eos2->isotherm(state2_ref);
    if( Teos2 == NULL )
        cerr << Error("Teos2 is NULL") << Exit;

    double P0 = max(P1_ref,P2_ref);
    double dP = (Pmax-P0)/nsteps;
    int k;
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(10) <<  " V"
         << " "  << setw(10) << "  e"
         << " "  << setw(10) << "  P"
         << " "  << setw(10) << "  V1"
         << " "  << setw(10) << "  e1"
         << " "  << setw(10) << "  V2"
         << " "  << setw(10) << "  e2"
         << "   count\n";
	cout.setf(ios::right, ios::adjustfield);
    for( k=0; k<=nsteps; k++ )
    {
        double P = P0 + k*dP;
        ThermalState state1;
        ThermalState state2;
        if( Teos1->P(P,state1) )
        {
            cerr << Error("Teos1 failed\n");
            if( k>0 )
            {
                P = P0 + (k-1)*dP;
                Teos1->P(P,state1);
                double KT = eos1->KT(state1.V,state1.e);
                cerr << "cT = " << sqrt(state1.V*KT) << "\n";
            }
            return 1;
        }
        if( Teos2->P(P,state2) )
        {
            cerr << Error("Teos2 failed\n");
            if( k>0 )
            {
                P = P0 + (k-1)*dP;
                Teos2->P(P,state2);
                double KT = eos2->KT(state2.V,state2.e);
                cerr << "cT = " << sqrt(state2.V*KT) << "\n";
            }
            return 1;
        }
        
        double V = lambda1*state1.V + lambda2*state2.V;
        double e = lambda1*state1.e+lambda2*state2.e;
        cout << setw(10) << setprecision(4) << V
             << " "  << setw(10) << setprecision(4) << e
             << " "  << setw(10) << setprecision(4) << P
             << " "  << setw(10) << setprecision(4) << state1.V
             << " "  << setw(10) << setprecision(4) << state1.e
             << " "  << setw(10) << setprecision(4) << state2.V
             << " "  << setw(10) << setprecision(4) << state2.e
             << "\n";
        double eosP = eos->P(V,e);
        cout << setw(10) << " "
             << " "  << setw(10) << " "
             << " "  << setw(10) << setprecision(4) << eosP
             << " "  << setw(10) << setprecision(4) << eosPT->V1
             << " "  << setw(10) << setprecision(4) << eosPT->e1
             << " "  << setw(10) << setprecision(4) << eosPT->V2
             << " "  << setw(10) << setprecision(4) << eosPT->e2
             << " "  << setw(7)  << eosPT->count
             << "\n";
    }
    P0 = min(P1_ref,P2_ref);
    double P1 = max(P1_ref,P2_ref);
    dP = (P1-P0)/nsteps;
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(10) <<  " V"
         << " "  << setw(10) << "  e"
         << " "  << setw(10) << "  P"
         << " "  << setw(10) << "  V1"
         << " "  << setw(10) << "  e1"
         << " "  << setw(10) << "  V2"
         << " "  << setw(10) << "  e2"
         << "   count\n";
	cout.setf(ios::right, ios::adjustfield);
    for( k=0; k<=nsteps; k++ )
    {
        double P = P1 - k*dP;
        ThermalState state1;
        ThermalState state2;
        if( Teos1->P(P,state1) )
        {
            cerr << Error("Teos1 failed\n");
            if( k>0 )
            {
                P = P1 - (k-1)*dP;
                Teos1->P(P,state1);
                double KT = eos1->KT(state1.V,state1.e);
                cerr << "cT = " << sqrt(state1.V*KT) << "\n";
            }
            return 1;
        }
        if( Teos2->P(P,state2) )
        {
            cerr << Error("Teos2 failed\n");
            if( k>0 )
            {
                P = P1 - (k-1)*dP;
                Teos2->P(P,state2);
                double KT = eos2->KT(state2.V,state2.e);
                cerr << "cT = " << sqrt(state2.V*KT) << "\n";
            }
            return 1;
        }
        double V = lambda1*state1.V + lambda2*state2.V;
        double e = lambda1*state1.e+lambda2*state2.e;
        cout << setw(10) << setprecision(4) << V
             << " "  << setw(10) << setprecision(4) << e
             << " "  << setw(10) << setprecision(4) << P
             << " "  << setw(10) << setprecision(4) << state1.V
             << " "  << setw(10) << setprecision(4) << state1.e
             << " "  << setw(10) << setprecision(4) << state2.V
             << " "  << setw(10) << setprecision(4) << state2.e
             << "\n";
        double eosP = eos->P(V,e);
        cout << setw(10) << " "
             << " "  << setw(10) << " "
             << " "  << setw(10) << setprecision(4) << eosP
             << " "  << setw(10) << setprecision(4) << eosPT->V1
             << " "  << setw(10) << setprecision(4) << eosPT->e1
             << " "  << setw(10) << setprecision(4) << eosPT->V2
             << " "  << setw(10) << setprecision(4) << eosPT->e2
             << " "  << setw(7)  << eosPT->count
             << "\n";
    }
    P0 = min(P1_ref,P2_ref);
    dP = (P0-Pmin)/nsteps;
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(10) <<  " V"
         << " "  << setw(10) << "  e"
         << " "  << setw(10) << "  P"
         << " "  << setw(10) << "  V1"
         << " "  << setw(10) << "  e1"
         << " "  << setw(10) << "  V2"
         << " "  << setw(10) << "  e2"
         << "   count"
         << "\n";
	cout.setf(ios::right, ios::adjustfield);
    for( k=0; k<=nsteps; k++ )
    {
        double P = P0 - k*dP;
        ThermalState state1;
        ThermalState state2;
        if( Teos1->P(P,state1) )
        {
            cerr << Error("Teos1 failed\n");
            if( k>0 )
            {
                P = P0 - (k-1)*dP;
                Teos1->P(P,state1);
                double KT = eos1->KT(state1.V,state1.e);
                cerr << "cT = " << sqrt(state1.V*KT) << "\n";
            }
            return 1;
        }
        if( Teos2->P(P,state2) )
        {
            cerr << Error("Teos2 failed\n");
            if( k>0 )
            {
                P = P0 - (k-1)*dP;
                Teos2->P(P,state2);
                double KT = eos2->KT(state2.V,state2.e);
                cerr << "cT = " << sqrt(state2.V*KT) << "\n";
            }
            return 1;
        }
        double V = lambda1*state1.V + lambda2*state2.V;
        double e = lambda1*state1.e+lambda2*state2.e;
        cout << setw(10) << setprecision(4) << V
             << " "  << setw(10) << setprecision(4) << e
             << " "  << setw(10) << setprecision(4) << P
             << " "  << setw(10) << setprecision(4) << state1.V
             << " "  << setw(10) << setprecision(4) << state1.e
             << " "  << setw(10) << setprecision(4) << state2.V
             << " "  << setw(10) << setprecision(4) << state2.e
             << "\n";
        double eosP = eos->P(V,e);
        cout << setw(10) << " "
             << " "  << setw(10) << " "
             << " "  << setw(10) << setprecision(4) << eosP
             << " "  << setw(10) << setprecision(4) << eosPT->V1
             << " "  << setw(10) << setprecision(4) << eosPT->e1
             << " "  << setw(10) << setprecision(4) << eosPT->V2
             << " "  << setw(10) << setprecision(4) << eosPT->e2
             << " "  << setw(7)  << eosPT->count
             << "\n";
    }
    deleteEOS(eos);
    return 0;
}
