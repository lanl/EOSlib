#include <Arg.h>

#include <PTequilibrium.h>
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

    double umax = 5.0;
    double nsteps = 10;
    EOS::Init();
    double e = NaN;
    double V = NaN;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(umax,umax);
		GetVar(e,e);
		GetVar(V,V);
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
    //
	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);
	cout.setf(ios::left, ios::adjustfield);
    //
    PTequilibrium *eosPT = dynamic_cast<PTequilibrium*>(eos);
    if( eosPT == NULL )
        cerr << Error("eosPT is NULL") << Exit;
    if( std::isnan(V) )
        V = eos->V_ref;
    if( std::isnan(e) )
        e = eos->e_ref;
    EOS *eos1 = eosPT->eos1;
    EOS *eos2 = eosPT->eos2;
    double &V1 = eosPT->V1;
    double &e1 = eosPT->e1;
    double &V2 = eosPT->V2;
    double &e2 = eosPT->e2;
    HydroState state0(V,e);
    double c0 = eosPT->c(V,e);
    //cout << "V,e,c = " << V << ", " << e << ", " << c0 << "\n";
    //Hugoniot *SPT = eosPT->EOS::shock(state0);
    Hugoniot *SPT = eosPT->shock(state0);
    if( SPT == NULL )
        cerr << Error("SPT is NULL") << Exit;
    WaveStateLabel(cout << "\nHugoniot\n")
        << " " <<setw(10) << " T"
        << " " <<setw(10) << " S"
        << "\n";
    eosPT->verbose=1;
    int k;
    for( k=0; k<=nsteps; k++ )
    {
        double u = k*(umax*c0)/double(nsteps);
        WaveState wave;
        if( SPT->u(u,RIGHT,wave) )
            cerr << Error("SPT->P failed\n") << Exit;
        double S = eosPT->S(wave.V,wave.e);
        double T = eosPT->T(wave.V,wave.e);
        cout << wave 
             << " " << T
             << " " << S
             << "\n";
        double KT1 = eos1->KT(V1,e1);
        double KT2 = eos2->KT(V2,e2);
        if( KT1 <= 0.0 || KT2 <= 0. )
            cout << "\tDomain error: KT1=" << KT1 << ", KT2=" << KT2 << "\n";
    }
    delete SPT;
    deleteEOS(eos);
    return 0;
}
