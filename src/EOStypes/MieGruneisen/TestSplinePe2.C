#include <Arg.h>

#include <PeSpline.h>

int main(int, char **argv)
{
	ProgName(*argv);
    EOS::Init();

    const char *file = "Test.data";
	const char *type = "PeSpline";
	const char *name = "PMMA-hydro";
	const char *units = NULL;

    int nsteps = 10;
	int dir = RIGHT;
    double umin = 0.;
    double umax = 0;

	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(nsteps,nsteps);
		GetVarValue(left,dir,LEFT);
		GetVarValue(right,dir,RIGHT);
        		
		ArgError;
	}
	cout.setf(ios::showpoint);    
    cout.setf(ios::scientific,ios::floatfield);
    Format S_form(12,4);
    V_form.width = 12;
    e_form.width = 12;
    u_form.width = 12;
    P_form.width = 12;
    T_form.width = 12;

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


    PeSpline *ptrPe = dynamic_cast<PeSpline*>(eos);
	if( ptrPe == NULL )
		cerr << Error("dynamic_cast failed") << Exit;
    PeSpline &solid = *ptrPe;
    solid.P_vac = solid.Pmin;
	
    double V1 = solid.Vmin;
    double Vmax = solid.Vmax;
    
	double V0 = solid.V_ref;
	double e0 = solid.e_ref;
	double u0 = 0;
    double c0 = solid.c(V0,e0);
	HydroState state0(V0,e0,u0);
	
    int i;
	double Nsteps = (double) nsteps;
// Test Hugoniot
	Hugoniot *H1 = solid.shock(state0);
	if( H1 == NULL )
		cerr << Error("H1 is NULL") << Exit;
	cout << "\nTest Hugoniot\n";	
	WaveStateLabel(cout) << " " << Center(T_form, "T")
                         << " " << Center(S_form, "S")
                         << "\n";
	const Units *u = eos->UseUnits();
	if( u )
	    WaveStateLabel(cout, *u) << " "
	                             << Center(T_form,u->Unit("temperature")) << " "
	                             << Center(S_form,u->Unit("specific_heat")) << " "
                                 << "\n";
    if( umax <= 0 )
        umax = 0.5*c0;
    for(i=0; i<=nsteps; i++ )
	{
        double u = umin + (i/Nsteps)*(umax-umin);
		WaveState w1;
		if( H1->u(u,dir,w1) )
			cerr << Error("H1->u failed") << Exit;
        double eta = w1.u/w1.us;
		cout << w1
             << " " << T_form << solid.EOS::T(w1) 
             << " " << S_form << solid.EOS::S(w1) 
             << "\n";
	}
	delete H1;
// Test Isentrope
	Isentrope *S1 = solid.isentrope(state0);
	if( S1 == NULL )
		cerr << Error("S1 is NULL") << Exit;	
    double V, f;
// Compressive branch
    cout << "\nTest Isentrope (compressive branch)\n";
	WaveStateLabel(cout) << " " << Center(T_form, "T")
                         << " " << Center(S_form, "S")
                         << "\n";
	if( u )
	    WaveStateLabel(cout, *u) << " "
	                             << Center(T_form,u->Unit("temperature")) << " "
	                             << Center(S_form,u->Unit("specific_heat")) << " "
                                 << "\n";
	double Vmin = 0.5*(V0+V1);
	V = V0;	
    f = pow(Vmin/V0, 1./Nsteps);
	for( i=0; i <=nsteps; i++, V *=f )
	{
		WaveState w1;
		if( S1->V(V,dir,w1) )
			cerr << Error("S1->V failed") << Exit;
		
		cout << w1 << " "
             << T_form << solid.EOS::T(w1)
             << S_form << solid.EOS::S(w1)
             << "\n";
	}
// Expansive branch
	cout << "\nTest Isentrope (expansive branch)\n";
	WaveStateLabel(cout) << " " << Center(T_form, "T")
                         << " " << Center(S_form, "S")
                         << " " << Center(u_form, "c")
                         << "\n";
	if( u )
	    WaveStateLabel(cout, *u) << " "
	                             << Center(T_form,u->Unit("temperature"))   << " "
	                             << Center(S_form,u->Unit("specific_heat")) << " "
	                             << Center(S_form,u->Unit("velocity"))      << " "
                                 << "\n";
	V = V0;	
    f = pow(Vmax/V0, 1./Nsteps);
	for( i=0; i <nsteps; i++, V *=f )
	{
		WaveState w1;
		if( S1->V(V,dir,w1) )
			cerr << Error("S1->V failed at V=") << V << Exit;
		
		cout << w1 << " "
             << T_form << solid.EOS::T(w1)
             << S_form << solid.EOS::S(w1)
             << u_form << solid.EOS::c(w1)
             << "\n";
	}
	delete S1;

    deleteEOS(eos);
    return 0;
}
