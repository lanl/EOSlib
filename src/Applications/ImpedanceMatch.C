#include <Arg.h>
#include <EOS.h>
#include <Riemann_gen.h>
#include <Porous.h>

void Header(int phi, const Units *u)
{
	int n = 10 + 6;
	n += V_form.width + e_form.width + u_form.width + P_form.width
		+ us_form.width + c_form.width + T_form.width;
	int i;
	for(i=0; i<n;i++)
		cout << "-";
	cout << "\n";
	cout << setw(13) << "";
	WaveStateLabel(cout) << Center(c_form,"c") << " "
		             << Center(T_form,"T") << " ";
	if( phi )
		cout << setw(10) << "phi  " << "\n";
	else
		cout << "\n";
		
	if( u )
	{
		cout << setw(13) << "";
		WaveStateLabel(cout, *u) << Center(u_form, u->Unit("velocity")) << " "
			           << Center(T_form, u->Unit("temperature")) << " "
				       << "\n";
	}
	for(i=0; i<n;i++)
		cout << "-";
	cout << "\n";
}

void PrintWave(const char* type, EOS *eos, WaveState &wave,
	       			 EqPorous *phi_eq = NULL)
{
	double c = sqrt(eos->c2(wave));
	double T = eos->T(wave);	
	
	
	cout.setf(ios::left,ios::adjustfield);
	cout << setw(10) << type;

	cout.setf(ios::right,ios::adjustfield);	
	cout << wave << " "
	     << c_form << c << " "
	     << T_form << T ;
	if( phi_eq )
		cout << setw(10) << setprecision(4) << phi_eq->phi(wave);
	cout << "\n";
}

void PrintState(EOS *eos, HydroState &state, EqPorous *phi_eq = NULL)
{
	WaveState wave;
	eos->Evaluate(state, wave);
	PrintWave("", eos, wave, phi_eq);
}

//

#define NaN EOS::NaN
int main(int, char **argv)
{
	ProgName(*argv);
    EOS_Init();
	
	double u_p  = NaN;	// piston velocity
	double Ps   = NaN;	// or shock pressure
	double u0   = 0.0;	// or flyer velocity
	
	const char *files = "EOS.data";
	const char *units = "hydro::std";

	const char *mat1 = NULL;
	double V1  = NaN;
	double e1  = NaN;
	double u1  = NaN;

	const char *mat2 = NULL;
	double V2  = NaN;
	double e2  = NaN;
	double u2  = NaN;

	while(*++argv)
	{
		GetVar(file,files);
		GetVar(files,files);
		GetVar(units,units);
		
		GetVar(mat1,mat1);
		GetVar2(state1,V1,e1);
		GetVar(V1,V1);
		GetVar(e1,e1);
		GetVar(u1,u1);
		
		GetVar(mat2,mat2);
		GetVar2(state2,V2,e2);
		GetVar(V2,V2);
		GetVar(e2,e2);
		GetVar(u2,u2);

		if( !strcmp(*argv,"u_p") )
		{
			Ps = NaN;
			u0 = 0.0;
		}
		GetVar(u_p,u_p);

		if( !strcmp(*argv,"Ps") )
		{
			u_p = NaN;
			u0 = 0.0;
		}
		GetVar(Ps,Ps);

		if( !strcmp(*argv,"u0") )
		{
			u_p = NaN;
			Ps = NaN;
		}
		GetVar(u0,u0);
		ArgError;
	}
	cout.setf(ios::showpoint);


// Material data base
	DataBase db;
	if( db.Read(files) )
	    cerr << Error("failed reading files: ") << files << Exit;
	
	Units *Cunits = db.FetchUnits(units);
	if( Cunits == NULL )
		cerr << Error("Could not FetchUnits, ") << units << Exit;

// eos1 and state1	
	const char *type = NULL;
	const char *name = NULL;
	if( mat1 == NULL )
		mat1 = "Hayes::HMX";
	TypeName(mat1, type, name);
	EOS *eos1 = FetchEOS(type, name, db);
	if( !eos1 )
		cerr << Error("material 1 `") << mat1 << "' not found" << Exit;
	if( eos1->ConvertUnits(*Cunits) )
		cerr << Error("ConvertUnits for `") << mat1 << "' failed" << Exit;
	HydroState state1;		
	state1.u = std::isnan(u1) ? u0 : u1;
	state1.V = std::isnan(V1) ? eos1->V_ref : V1;
	state1.e = std::isnan(e1) ? eos1->e_ref : e1;

// eos2 and state2	
	if( mat2 == NULL )
		mat2 = "Hayes::TPX";
	TypeName(mat2, type, name);
	EOS *eos2 = FetchEOS(type, name, db);
	if( !eos2 )
		cerr << Error("material 2 `") << mat2 << "' not found" << Exit;
	if( eos2->ConvertUnits(*Cunits) )
        cerr << Error("ConvertUnits for `") << mat2 << "' failed" << Exit; 
	HydroState state2;		
	state2.u = std::isnan(u2) ? 0 : u2;
	state2.V = std::isnan(V2) ? eos2->V_ref : V2;
	state2.e = std::isnan(e2) ? eos2->e_ref : e2;

// phi_eq
	EqPorous *phi_eq1 = dynamic_cast<EqPorous *>(eos1);
	EqPorous *phi_eq2 = dynamic_cast<EqPorous *>(eos2);

// header
	cout << "Material 1: " << mat1 << "\n";
	PrintState(eos1,state1,phi_eq1);

	cout << "Material 2: " << mat2 << "\n";
	PrintState(eos2,state2,phi_eq2);

	Header(phi_eq1 != NULL || phi_eq2 != NULL, Cunits);
	                     
// incident shock	
	Hugoniot *H_in = eos1->shock(state1);	
	WaveState incident;
	if( !std::isnan(Ps) )
	{
		if( H_in->P(Ps,RIGHT,incident) )
			cerr << Error("H_in->P failed") << Exit;
	}
	else
	{
		if( std::isnan(u_p) )
			u_p = state1.u;
		
		if( H_in->u(u_p,RIGHT,incident) )
			cerr << Error("H_in->u failed") << Exit;
	}
	delete H_in;


	PrintWave("incident: ", eos1, incident, phi_eq1);
	
// Riemann problem		
	RiemannSolver_generic RP(*eos1, *eos2);	
	WaveState reflect, transmit;
	
	if( RP.Solve(incident,state2, reflect, transmit) )
		cerr << Error("RP.Solve failed") << Exit;
		
	PrintWave("reflect: ", eos1, reflect, phi_eq1);
	PrintWave("transmit: ", eos2, transmit, phi_eq2);

	deleteEOS(eos1);
	deleteEOS(eos2);
    deleteUnits(Cunits);

	return 0;
}
