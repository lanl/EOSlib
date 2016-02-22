#include <Arg.h>
#include <EOS.h>


int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "BirchMurnaghan";
	const char *name = "HMX";
	const char *units = NULL;
	int nsteps = 10;

    double T0 = 300;
    double P0 = 1e-4;
    double T1 = 500;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(type,type);
		GetVar(units,units);
        
		GetVar(P0,P0);
		GetVar(T0,T0);
		GetVar(T1,T1);
		GetVar(nsteps,nsteps);

	    ArgError;
	}
    
// set eos and initial state
	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;
	
	EOS *eos = FetchEOS(type,name,db);
	if( !eos )
	    cerr << Error("material not found, ") << type << "::" << name << Exit;
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;
    
	HydroState state0;        
	if( eos->PT(P0,T0,state0) )
	    cerr << Error("eos->PT(P0,T0) failed\n") << Exit;


	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);
	cout.setf(ios::left, ios::adjustfield);

    cout << setw(8)  << " T"    << " "
         << setw(10) << " beta" << " "
         << setw(10) << " Cp" << " "
         << setw(10) << " c" << " "
         << "\n";


    double T;
	double dT = (T1-T0)/nsteps;
	HydroState state;        

	int count;    
	for(count=nsteps+1, T=T0; count--; T += dT)
	{
	    if( eos->PT(P0,T, state)  )
	        cerr << Error("eos->PT failed")
                 << " at T = " << T << "\n" << Exit;
        double beta = eos->beta(state.V,state.e);
        double Cp = eos->CP(state.V,state.e);
        double c  = eos->c(state.V,state.e);
        cout << setprecision(2) << T << " "
             << setprecision(4) << beta << " "
             << setprecision(4) << Cp << " "
             << setprecision(4) << c << " "
             << "\n";
	}
	return 0;
}
    
