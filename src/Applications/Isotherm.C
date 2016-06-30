#include <Arg.h>
#include <EOS.h>
#include <Porous.h>

     EOS *eos    = NULL;
EqPorous *phi_eq = NULL;

Format FD_form;
Format phi_form;

void PrintState(ThermalState &state)
{            
	double c = sqrt(eos->c2(state.V,state.e));
	double FD = eos->FD(state.V,state.e);
    double KT = eos->KT(state.V,state.e);
    double cT = sqrt(state.V*KT);
    //
    //double Gamma = eos->Gamma(state.V,state.e);
    //double Cv = eos->CV(state.V,state.e);
	//   
	cout << state << " "
	     << c_form  << c << " "
	     << FD_form << FD << " "
         << c_form  << cT;
	if( phi_eq )
	{
	    double phi = phi_eq->phi(state.P*state.V);
	    cout << " " << phi_form << phi
	         << " " << e_form   << phi_eq->energy(phi);
	}
    //
    //cout << " " <<  FD_form << Gamma << " " <<  FD_form << Cv;
    //
	cout << "\n";
}

void PrintLine()
{
	cout.fill('-');
	cout << setw(V_form.width)  << "" << "-"
	     << setw(e_form.width)  << "" << "-"
	     << setw(P_form.width)  << "" << "-"
	     << setw(T_form.width)  << "" << "-"
	     << setw(c_form.width)  << "" << "-"
	     << setw(FD_form.width) << "" << "-"
	     << setw(c_form.width)  << "" << "\n";
	cout.fill(' ');
}

const char *help[] = {    // list of commands printed out by help option
	"name        name    # material name [HMX]",
	"type        name    # EOS type [Hayes]",
	"material    name    # type::name",
	"file[s]     file    # : separated list of data files",
	"units       name    # default units from data base",
	"",
	"V0     value   # specific volume [V_ref] of reference state",
	"e0     value   # specific energy [e_ref] of reference state",
	"state  V0 e0   # reference state",
	"P0     value   # pressure of reference state",
	"T0     value   # Temperature of reference state",
	"PT     P0 T0   # reference state",
	"",
	"InitShock      # Initial state on shock locus based on ref state",
	"InitIsentrope  # Initial state on isentrope from ref state",
	"u_p   value    # shock particle velocity",
	"u_s   value    # shock velocity",
	"Ps    value    # shock pressure",
	"Vs    value    # shock specific volume",
	"",
	"P              # isotherm curve by P",
	"V              # isotherm curve by V",
	"",
	"range   var1 var2    # range of wave curve",
	"var1    value,       # default is shock state",
	"var2    value",
	"",
	"P2    value    # P  & var2=value",
	"V2    value    # V  & var2=value",
	"",
	"nsteps    value    # number of points on wave curve",
	0
};

void Help(int status)
{
	const char **list;
	for(list=help ;*list; list++)
	{
	    cerr << *list << "\n";
	}
	exit(status);
}

#define NaN EOS::NaN
int main(int, char **argv)
{
	ProgName(*argv);
    EOS_Init();
	
	int nsteps = 10;

	int start = 1;        // 1 for init shock, 2 for init isentrope
	
	double Vs   = NaN;    // shock or isentrope V
	double Ps   = NaN;    // shock or isentrope P
	
	double V0 = NaN;
	double e0 = NaN;
	double u0 = NaN;
	double P0 = NaN;
	double T0 = NaN;
	
	const char *files = "EOS.data";    
	const char *type     = NULL;
	const char *name     = NULL;
	const char *material = NULL;
	const char *units    = "hydro::std";
	
	double var1 = NaN;
	double var2 = NaN;
	
	int loop_P = 0;
	int loop_V = 0;
	
// process command line arguments
	if( argv[1] == NULL )
	    Help(-1);    
	while(*++argv)
	{
	    if( !strcmp(*argv,"Ps") )
	        Vs = NaN;
	    GetVar(Ps,Ps);

	    if( !strcmp(*argv,"Vs") )
	        Ps = NaN;
	    GetVar(Vs,Vs);
	    
	    GetVar(nsteps,nsteps);
	    
	    GetVar(file,files);
	    GetVar(files,files);
	    GetVar(type,type);
	    GetVar(name,name);
	    GetVar(material,material);
	    GetVar(units,units);
	    
	    GetVar(V0,V0);
	    GetVar(e0,e0);
	    GetVar(u0,u0);
	    GetVar2(state, V0, e0);

	    GetVar(P0,P0);
	    GetVar(T0,T0);
	    GetVar2(PT, P0, T0);

	    GetVar2(range,var1,var2);
	    GetVar(var1,var1);
	    GetVar(var2,var2);
	    
	    if( !strcmp(*argv,"P") )
	    {
	        loop_P = 1;
	        loop_V = 0;
	        continue;
	    }
	    if( !strcmp(*argv,"P2") )
	    {
	        loop_P = 1;
	        loop_V = 0;
	    }
	    GetVar(P2,var2);
	    
	    if( !strcmp(*argv,"V") )
	    {
	        loop_V = 1;
	        loop_P = 0;
	        continue;
	    }
	    if( !strcmp(*argv,"V2") )
	    {
	        loop_V = 1;
	        loop_P = 0;
	    }
	    GetVar(V2,var2);

	    GetVarValue(InitShock,    start, 1);
	    GetVarValue(InitIsentrope,start, 2);

	    if( !strcmp(*argv, "V_form") )
	    {
	        char *ptr;
	        V_form.width = strtol(*++argv,&ptr,10);
	        if( *ptr != '\0' )
	           cerr << Error("following command line argument V_form")
			<< Exit;
	        V_form.precision = strtol(*++argv,&ptr,10);
	        if( *ptr != '\0' )
	           cerr << Error("following command line argument V_form")
			<< Exit;
	                
	        continue;
	    }
	    if( !strcmp(*argv, "e_form") )
	    {
	        char *ptr;
	        e_form.width = strtol(*++argv,&ptr,10);
	        if( *ptr != '\0' )
	           cerr << Error("following command line argument V_form")
			<< Exit;
	        e_form.precision = strtol(*++argv,&ptr,10);
	        if( *ptr != '\0' )
	           cerr << Error("following command line argument V_form")
			<< Exit;
	                
	        continue;
	    }
	    if( !strcmp(*argv, "u_form") )
	    {
	        char *ptr;
	        u_form.width = strtol(*++argv,&ptr,10);
	        if( *ptr != '\0' )
	           cerr << Error("following command line argument V_form")
			<< Exit;
	        u_form.precision = strtol(*++argv,&ptr,10);
	        if( *ptr != '\0' )
	           cerr << Error("following command line argument V_form")
			<< Exit;
	                
	        continue;
	    }

	    if( !strcmp(*argv, "?") || !strcmp(*argv,"help") )
	        Help(0);
	    ArgError;
	}
	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);
	
	if( (loop_P || loop_V) && std::isnan(var2) )
	    cerr << Error("var2 not set") << Exit;
	
	if( material )
	{
	    if( type || name )
		    cerr << Error("Can not specify both material and name or type")
	             << Exit;
	    if( TypeName(material,type,name) )
		    cerr << Error("syntax error, material = ") << material
		         << Exit;
	}
	else
	{
	    if( type == NULL )
		    type = "Hayes";
	    if( name == NULL )
		    name = "HMX";
	}
	
// set eos and initial state
	DataBase db;
	if( db.Read(files) )
		cerr << Error("Read failed" ) << Exit;
	
	eos = FetchEOS(type,name,db);
	if( !eos )
	    cerr << Error("material not found, ") << type << "::" << name << Exit;    
	if( eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;

	HydroState state0;        
	state0.u = 0;
	state0.V = std::isnan(V0) ? eos->V_ref : V0;
	state0.e = std::isnan(e0) ? eos->e_ref : e0;

	if( !std::isnan(P0) || !isnan(T0) )
	{
	    if( std::isnan(P0) )
	        P0 = eos->P(state0);
	    if( std::isnan(T0) )
	        T0 = eos->T(state0);
	    if( eos->PT(P0,T0,state0) )
	        cerr << Error("eos->PT failed\n") << Exit;
	}
	else
	    if( std::isnan(P0 = eos->P(state0)) )
	        cerr << Error("eos->P returned NaN\n") << Exit;
	
// incident shock or isentrope
	Wave *H;
	if( start == 1 )
	    H = eos->shock(state0);
	else
	    H = eos->isentrope(state0);

	WaveState incident;
	if( !std::isnan(Ps) )
	{
	    if( H->P(Ps,RIGHT,incident) )
	        cerr << Error("H->P failed") << Exit;
	}
	else if( !std::isnan(Vs) )
	{
	    if( H->V(Vs,RIGHT,incident) )
	        cerr << Error("H->V failed") << Exit;
	}
	else
	    eos->Evaluate(state0,incident);
	    
	
// select locus    variable P or V   
	Isotherm *wave = eos->isotherm(incident);
    if( wave == NULL )
	        cerr << Error("eos->isotherm failed") << Exit;
	int (Isotherm::*Locus)(double,ThermalState&);
	
	if( loop_P )
	{
	    Locus = &Isotherm::P;
	    if( std::isnan(var1) )
	        var1 = incident.P;
	}
	else if( loop_V )
	{
	    Locus = &Isotherm::V;
	    if( std::isnan(var1) )
	        var1 = incident.V;
	}
	    
// print header
	cout << "Isotherm for " << type << "::" << name << "\n";
	     
	ThermalState Tstate;    
	if( incident.u != state0.u )
	{
	    eos->Evaluate(state0, Tstate);
	    PrintState(Tstate);
	    PrintLine();    
	}
	ThermalStateLabel(cout) << " "
	                     << Center(c_form, "c")  << " "
	                     << Center(FD_form,"FD") << " "
	                     << Center(c_form, "cT");
	int extra = 0;
	if( (phi_eq=dynamic_cast<EqPorous *>(eos)) )
	{
	    cout << " " << Center(phi_form,"phi") << " " << Center(e_form,  "B");
	    extra = 1;
	}
	cout << "\n";
	const Units *u = eos->UseUnits();
	if( u )
	{
	    ThermalStateLabel(cout, *u) << " "
	                             << Center(u_form,u->Unit("velocity")) << " "
	                             << Center(FD_form,"-") << " "
	                             << Center(u_form,u->Unit("velocity"));
	        if( extra )
	            cout << " " << Center(phi_form," ")
	                 << " " << Center(e_form,u->Unit("specific_energy"));
	        cout << "\n";
	}
	PrintLine();
	
	if( std::isnan(var2) )
    {
	    if( incident.u == state0.u )
	        eos->Evaluate(state0, Tstate);
        else
	        eos->Evaluate(incident, Tstate);
    }
    else
    { // print wave locus        
    	double var;
    	double dvar = (var2-var1)/nsteps;
    
    	int count;    
    	for(count=nsteps, var=var1; count--; var += dvar)
    	{
    	    if( (wave->*Locus)(var,Tstate) )
    	        cerr << Error("wave->locus failed") << Exit;
    	    PrintState(Tstate);
    	}
    	// avoid round off error on var = var2
    	if( (wave->*Locus)(var2,Tstate) )
    	    cerr << Error("wave->locus failed") << Exit;
    }
    PrintState(Tstate);
	cout.flush();
	
// test reference counting
	delete wave;        
	delete H;
	deleteEOS(eos);
	    
	return 0;
}
