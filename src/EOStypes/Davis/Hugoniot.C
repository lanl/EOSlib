#include <Arg.h>

#include <EOS.h>

int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "DavisReactants";
	const char *name = "PBX9501";
	const char *units = NULL;
    double Pmax = 100.;
    int nsteps = 10;
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
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
	EOS *reactants = FetchEOS(type,name,db);
	if( reactants == NULL )
		cerr << Error("Fetch failed") << Exit;
    if( units && reactants->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed reactants") << Exit;

    double V0 = reactants->V_ref;
    double e0 = reactants->e_ref;
    double P0 = reactants->P(V0,e0);
    HydroState state0(V0,e0);
    //
    Hugoniot *shock = reactants->shock(state0);
    if( shock == NULL )
        cerr << Error("reactants->shockonation failed" ) << Exit;

    cout << "V0=" << V0 << "\n";
	cout.setf(ios::left, ios::adjustfield);
    cout        << setw(13) << "#  P"
         << " " << setw(13) << "   V"
         << " " << setw(13) << "   e"
         << " " << setw(13) << "   u"
         << " " << setw(13) << "   us"
         << " " << setw(13) << "   c"
         << " " << setw(13) << "   T"
         << " " << setw(13) << "   S"
         << " " << setw(13) << "   Cv"
         << "\n";
	cout.setf(ios::showpoint);
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::scientific, ios::floatfield);
    int i;
    for( i=0; i<= nsteps; i++ )
    {
        double P = P0 + double(i)/double(nsteps) * (Pmax-P0);
        WaveState wave;
        if( shock->P(P,RIGHT,wave) )
            cerr << Error("shock->P failed" ) << Exit;
        double c = reactants->c(wave.V,wave.e);
        double T = reactants->T(wave.V,wave.e);        
        double S = reactants->S(wave.V,wave.e);        
        double Cv = reactants->CV(wave.V,wave.e);        
        //double FD = reactants->FD(wave.V,wave.e);        
        cout        << setw(13) << setprecision(6) << P
             << " " << setw(13) << setprecision(6) << wave.V
             << " " << setw(13) << setprecision(6) << wave.e
             << " " << setw(13) << setprecision(6) << wave.u
             << " " << setw(13) << setprecision(6) << wave.us
             << " " << setw(13) << setprecision(6) << c
             << " " << setw(13) << setprecision(6) << T
             << " " << setw(13) << setprecision(6) << S
             << " " << setw(13) << setprecision(6) << Cv
             //<< " " << setw(13) << setprecision(6) << FD
             << "\n";            
    }
    delete shock;
    deleteEOS(reactants);
    return 0;
}
