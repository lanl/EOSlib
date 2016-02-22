#include <Arg.h>
#include <EOS.h>


int main(int, char **argv)
{
	ProgName(*argv);
	const char *file     = "EOS.data";
	const char *material = "BirchMurnaghan::HMX";
	const char *units    = NULL;

    double T0  = 300;
    double P0  = 1e-4;
    double P1  = 0;
    double P2  = 20;
	int nsteps = 19;

    double fdissipation = 1.0;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(material,material);
		GetVar(units,units);
        GetVar(fdissipation,fdissipation);
        
        
		GetVar(P0,P0);
		GetVar(T0,T0);
		GetVar(P1,P1);
		GetVar(P2,P2);
		GetVar(nsteps,nsteps);

	    ArgError;
	}
    if( P1 < P0 ) P1 = P0;
    
// set eos and initial state
	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;
	
	EOS *eos = FetchEOS(material,db);
	if( !eos )
	    cerr << Error("material not found, ") << material << Exit;
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;
    
	HydroState state0;        
	if( eos->PT(P0,T0,state0) )
	    cerr << Error("eos->PT(P0,T0) failed\n") << Exit;
    state0.u = 0.0;

    Hugoniot *hugoniot = eos->shock(state0);
    if( hugoniot == NULL )
        cerr << Error("eos->shock failed") << Exit;
    Isentrope *isentrope = eos->isentrope(state0);
    if( isentrope == NULL )
        cerr << Error("eos->isentrope failed") << Exit;
    
	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);
	cout.setf(ios::left, ios::adjustfield);

    cout << Center(P_form, "P")   << " "
         << Center(T_form, "T")   << "\n";
	const Units *u = eos->UseUnits();
    cout << Center(P_form, u->Unit("pressure"))    << " "
         << Center(T_form, u->Unit("temperature")) << "\n";
    

	double P = P1;
    double dP = (P2-P1)/nsteps;

    WaveState H;
    WaveState S;
    WaveState E;
    HydroState state;
    for( int count = nsteps+1; count--; P += dP )
    {
       if( hugoniot->P(P,RIGHT,H) )
	       cerr << Error("hugoniot failed\n") << Exit;
       state.V = H.V;
       if( isentrope->V(state.V,RIGHT,S) )
	       cerr << Error("isentrope failed\n") << Exit;
       state.e = S.e + fdissipation*(H.e-S.e);
       Isentrope *isentrope1 = eos->isentrope(state);
       if( isentrope1 == NULL )
           cerr << Error("eos->isentrope failed") << Exit;
       if( isentrope1->P(P,RIGHT,S) )
	       cerr << Error("isentrope failed\n") << Exit;
       delete isentrope1;
       
       cout << P_form << eos->P(S) << " "
            << T_form << eos->T(S) << "\n";
    }
    delete isentrope;
    delete hugoniot;
    
    return 0;
}   
