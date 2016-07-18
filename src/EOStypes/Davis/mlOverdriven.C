#include <Arg.h>

#include <EOS.h>

int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "DavisProducts";
	const char *name = "PBX9501";
	const char *units = NULL;
    double Pmax = 100.;
    int nsteps = 10;
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(type,type);
		GetVar(name,name);
		GetVar(units,units);
        GetVar(Pmax,Pmax);
        GetVar(nsteps,nsteps);        
		ArgError;
	}
	if( file == NULL )
		cerr << Error("Must specify data file") << Exit;
	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;
	EOS *products = FetchEOS(type,name,db);
	if( products == NULL )
		cerr << Error("Fetch failed") << Exit;
    if( units && products->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed products") << Exit;

    double P0  = 1.e-4;
    double V0 = products->V_ref;
    double e0 = products->e_ref;
    HydroState state0(V0,e0);
    //
    Detonation *det = products->detonation(state0,P0);
    if( det == NULL )
        cerr << Error("products->detonation failed" ) << Exit;
    WaveState CJ;
    if( det->CJwave(RIGHT,CJ) )
        cerr << Error("det->CJwave failed" ) << Exit;

    cout << "V0=" << V0 << " e0=" << e0 << "\n";
	cout.setf(ios::left, ios::adjustfield);
    cout        << setw(13) << "#  P"
         << " " << setw(13) << "   V"
         << " " << setw(13) << "   e"
         << " " << setw(13) << "   u"
         << " " << setw(13) << "   us"
         << " " << setw(13) << "   c"
         << " " << setw(13) << "   T"
         << " " << setw(13) << "   S"
         << "\n";
	cout.setf(ios::showpoint);
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::scientific, ios::floatfield);
    int i;
    for( i=0; i<= nsteps; i++ )
    {
        double P = CJ.P + double(i)/double(nsteps) * (Pmax-CJ.P);
        WaveState wave;
        if( det->P(P,RIGHT,wave) )
            cerr << Error("det->P failed" ) << Exit;
        double c = products->c(wave.V,wave.e);
        double T = products->T(wave.V,wave.e);        
        double S = products->S(wave.V,wave.e);        
        cout        << setw(13) << setprecision(6) << P
             << " " << setw(13) << setprecision(6) << wave.V
             << " " << setw(13) << setprecision(6) << wave.e
             << " " << setw(13) << setprecision(6) << wave.u
             << " " << setw(13) << setprecision(6) << wave.us
             << " " << setw(13) << setprecision(6) << c
             << " " << setw(13) << setprecision(6) << T
             << " " << setw(13) << setprecision(6) << S
             << "\n";            
    }
    delete det;
    deleteEOS(products);
    return 0;
}
