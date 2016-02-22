#include <Arg.h>
#include <EOS.h>
#include <Uniaxial.h>

void PrintState(WaveState &state, VonMisesElasticPlastic *eos)
{			
	double c = sqrt(eos->c2(state.V,state.e));
	double T = eos->T(state.V,state.e);
	
	double e = eos->ShearEnergy(state.V,state.e);
	double Ps = eos->P_solid(state.V, state.e-e);
		
	cout << state << setw(10) << setprecision(4) << c
	              << setw(10) << setprecision(4) << T
	              << setw(10) << setprecision(4) << Ps;
	        
	cout << "\n";
}

void PrintLine()
{
	int i;
	for(i=0; i<80;i++)
		cout << "-";
	cout << "\n";
}

#define NaN EOS::NaN
int main(int, char **argv)
{
	ProgName(*argv);
    EOS_Init();
	
	double V0 = NaN;
	double e0 = NaN;
	
	const char *files    = "EOS.data";	
	const char *type     = NULL;
    const char *name     = NULL;
	const char *material = NULL;
	const char *units    = "hydro::std";
	
// process command line arguments	
	while(*++argv)
	{

		GetVar(file,files);
		GetVar(files,files);
	    GetVar(type,type);
	    GetVar(name,name);
		GetVar(material,material);
		GetVar(units,units);
		
		GetVar(V0,V0);
		GetVar(e0,e0);
		GetVar2(state, V0, e0);
		
		ArgError;
	}
	cout.setf(ios::showpoint);
	
	if( material )
	{
	    if( type || name )
		    cerr << Error("Can not specify both material and name or type")
	             << Exit;
	    if( TypeName(material,type,name) )
		    cerr << Error("syntax error, material = ") << material << Exit;
	}
	else
	{
	    if( type == NULL )
		    type = "VonMisesElasticPlastic";
	    if( name == NULL )
		    name = "HMX";
	}
	
// set eos and initial state
	DataBase db;
	if( db.Read(files) )
		cerr << Error("Read failed" ) << Exit;

	EOS *eos = FetchEOS(type,name,db);
	if( eos == NULL )
	    cerr << Error("material not found, ") << type << "::" << name << Exit;    
	if( eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;
	

	// change VonMisesElasticPlastic to UniaxialEOS
	if( strcmp(type,"VonMisesElasticPlastic") )
		cerr << Error("material `") << type << "::" << name
		     << "' not UniaxialEOS"
		     << Exit;

	VonMisesElasticPlastic *EPeos = (VonMisesElasticPlastic *)eos;
	HydroState state0;		
	state0.u = 0;
	state0.V = isnan(V0) ? eos->V_ref : V0;
	state0.e = isnan(e0) ? eos->e_ref : e0;
	double P0 = eos->P(state0);
	
	cout << "Material " << eos->Name() << "::" << eos->Type() << "\n";
	     
// print header
	WaveState wstate;	
	eos->Evaluate(state0, wstate);
	PrintState(wstate, EPeos);
	PrintLine();	
	WaveStateLabel(cout) << setw(10) << "c    "
	                     << setw(10) << "T    "
	                     << setw(10) << "Ps    "
	                     << "\n";
	Units *u = eos->DupUnits();
	if( u )
	{
		WaveStateLabel(cout, *u) << setw(10) << u->Unit("velocity")
	        	<< setw(10) << u->Unit("temperature")
	        	<< setw(10) << u->Unit("pressure")
	                << "\n";
	}
	deleteUnits(u);
	PrintLine();

// elastic & plastic shocks	
	Hugoniot *H1 = eos->shock(state0);	
	WaveState elastic;
	double Vy = EPeos->Vy_compression(state0.V, state0.e);
	if( H1->V(Vy,RIGHT,elastic) )
		cerr << Error("H1->V failed") << Exit;
	PrintState(elastic, EPeos);
	double e = EPeos->ShearEnergy(elastic.V, elastic.e);
	double eps_y = log(state0.V/Vy);
	
	elastic.V *= 1.-1e-10;	// slightly above yield surface
				// in order for us to be supersonic
	Hugoniot *H2 = eos->shock(elastic);
	WaveState plastic;
	double us = elastic.us;
	if( H2->u_s(us,RIGHT,plastic) )
		cerr << Error("H2->u_s failed") << Exit;
	PrintState(plastic, EPeos);

	
	double dT = e/eos->CV(elastic.V, elastic.e);
	double dP = eos->Gamma(elastic.V, elastic.e)/elastic.V * e;
	cout << "\t  eps_y = " << eps_y << "\n"
	     << "\te_shear = " << e
	     << " \tdT=e_shear/Cv = " << dT
	     << " \tdP=(Gamma/V)e_shear = " << dP
	     << "\n";

// test reference counting
	delete H1;
	delete H2;
	deleteEOS(eos);
		
	return 0;
}
