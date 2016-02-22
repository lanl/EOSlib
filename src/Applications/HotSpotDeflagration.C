#include <Arg.h>
#include <HEburn.h>
#include <Riemann_gen.h>


int main(int, char **argv)
{
	ProgName(*argv);
    InitFormat();
    
	const char *file  = "HE.data";
	const char *type  = "HEburn";
	const char *name  = "PBX9501.BM.mod";
	const char *units = NULL;
    const char *lib   = "../lib/Linux";

    double P1  = 1;
    double P2  = 20;
	int nsteps = 19;

	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        GetVar(lib,lib);
               
		GetVar(P1,P1);
		GetVar(P2,P2);
		GetVar(nsteps,nsteps);

	    ArgError;
	}
#ifdef LINUX
      setenv("SharedLibDirEOS",lib,1);
#else
      putenv(Cat("SharedLibDirEOS=",lib));
#endif
	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;	
	EOS *eos = FetchEOS(type,name,db);
	if( !eos )
	    cerr << Error("material not found, ") << type << "::" << name << Exit;
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;
    HEburn *HE = dynamic_cast<HEburn *>(eos);
    if( HE == NULL )
        cerr << Error("HE is NULL") << Exit;
    EOS *Reactants = HE->Reactants();
    if( Reactants == NULL )
        cerr << Error("Reactants is NULL") << Exit;
    EOS *Products = HE->Products();
    if( Products == NULL )
        cerr << Error("Products is NULL") << Exit;
    double V0 = Reactants->V_ref;
    double e0 = Reactants->e_ref;
    double P0 = Reactants->P(V0,e0);
	HydroState state0(V0,e0);        
    Hugoniot *hugoniot = Reactants->shock(state0);
    if( hugoniot == NULL )
        cerr << Error("eos->shock failed") << Exit;
    RiemannSolver_generic RP(*Products,*Reactants);    
	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);
	cout.setf(ios::fixed, ios::floatfield);
	cout.setf(ios::left, ios::adjustfield);
	cout.setf(ios::right, ios::adjustfield);
    P_form.width = 7;
    P_form.precision = 1;
    T_form.width = 7;
    T_form.precision = 1;
    u_form.width = 6;
    u_form.precision = 2;

    cout << Center(P_form, "Ps")  << " "
         << Center(T_form, "Ts")  << " "
         << Center(P_form, "Phs") << " "
         << Center(T_form, "Ths") << " "
         << Center(P_form, "Prp") << " "
         << Center(T_form, "Trp") << " "
         << Center(P_form, "Pcj") << " "
         << Center(T_form, "Tcj") << " "
         << Center(u_form, "Dcj") << " "
         << Center(u_form, "D@Ps") << " "
         << "\n";    
	const Units *u = eos->UseUnits();
    cout << Center(P_form, u->Unit("P"))  << " "
         << Center(T_form, u->Unit("T")) << " "
         << Center(P_form, u->Unit("P")) << " "
         << Center(T_form, u->Unit("T")) << " "
         << Center(P_form, u->Unit("P")) << " "
         << Center(T_form, u->Unit("T")) << " "
         << Center(P_form, u->Unit("P")) << " "
         << Center(T_form, u->Unit("T")) << " "
         << Center(u_form, u->Unit("u")) << " "
         << Center(u_form, u->Unit("u")) << " "
         << "\n";
    WaveState shock;
    WaveState left, right;    
    Deflagration *defl;
    WaveState CJ;    
    WaveState Dwave;
    int i;
    for( i=0; i<=nsteps; i++)
    {
        double Ps = P1 + i*(P2-P1)/double(nsteps); 
        if( hugoniot->P(Ps,RIGHT,shock) )
	       cerr << Error("hugoniot failed") << Exit;
        double Ts  = Reactants->T(shock);
        double Phs = Products->P(shock);
        double Ths = Products->T(shock);
        shock.u = 0.0;
        if( RP.Solve(shock, shock, left,right) )
            cerr << Error("RP.Solve failed\n") << Exit;        
        double Prp = Reactants->P(right);
        double Trp = Reactants->T(right);
        defl = Products->deflagration(right,Prp);
        if( defl == NULL )
            cerr << Error("defl is NULL") << Exit;
        if( defl->CJwave(RIGHT,CJ) )
            cerr << Error("CJwave failed") << Exit;
        double Tcj = Products->T(CJ);
        cout << P_form << Ps    << " "
             << T_form << Ts    << " "
             << P_form << Phs   << " "
             << T_form << Ths   << " "
             << P_form << Prp   << " "
             << T_form << Trp   << " "
             << P_form << CJ.P  << " "
             << T_form << Tcj   << " "
             << u_form << CJ.us << " ";
        if( Ps > CJ.P )
        {
            if( defl->P(Ps,RIGHT,Dwave) )
                cerr << Error("defl->P failed") << Exit;    
            cout << u_form << Dwave.us << " ";
        }
        cout << "\n";
        delete defl;
    }
    deleteEOS(Reactants);
    deleteEOS(Products);
    delete hugoniot;
    deleteEOS(eos);
    return 0;
}
