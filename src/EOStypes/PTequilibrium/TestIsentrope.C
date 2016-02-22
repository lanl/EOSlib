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

    double Pmax = 50.0;
    double Pmin =  0.1;
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
        
		GetVar(Pmax,Pmax);
		GetVar(Pmin,Pmin);
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
    if( isnan(V) )
        V = eos->V_ref;
    if( isnan(e) )
        e = eos->e_ref;
    HydroState state0(V,e);
    Isentrope *SPT = eosPT->isentrope(state0);
    if( SPT == NULL )
        cerr << Error("SPT is NULL") << Exit;     
    WaveStateLabel(cout << "\nIsentrope\n")
        << " " <<setw(10) << " T"
        << " " <<setw(10) << " S"
        << "\n";
    int k;
    for( k=0; k<=nsteps; k++ )
    {
        double P = Pmin + k*(Pmax-Pmin)/double(nsteps);
        WaveState wave;
        if( SPT->P(P,RIGHT,wave) )
            cerr << Error("SPT->P failed\n") << Exit;
        double S = eosPT->S(wave.V,wave.e);
        double T = eosPT->T(wave.V,wave.e);
        cout << wave 
             << " " << T
             << " " << S
             << "\n";
    }
    delete SPT;
    deleteEOS(eos);
    return 0;
}
