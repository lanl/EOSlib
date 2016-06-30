#include <Arg.h>
#include <EOS.h>

int main(int, char **argv)
{
	ProgName(*argv);
    InitFormat();

	const char *files    = "EOS.data";    
    const char *reactant = "BirchMurnaghan::HMX";
    const char *products = "JWL::HMX";

    double P1 = 0;
    double P2 = 2;
    int nsteps = 10;
    int deflagration = 0;
    
    while(*++argv)
	{
	    GetVar(file,files);
	    GetVar(files,files);
	    GetVar(reactant,reactant);
	    GetVar(products,products);

        GetVar(P1,P1);
        GetVar(P2,P2);
        GetVar(nsteps,nsteps);
        GetVarValue(deflagration,deflagration,1);
     
	    ArgError;
	}

	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);

    DataBase db;
	if( db.Read(files) )
		cerr << Error("Read failed") << Exit;


    EOS *solid = FetchEOS(reactant,db);
    if( solid == NULL )
        cerr << Error("FetchEOS(reactant) failed") << Exit;
    
    
    EOS *gas = FetchEOS(products,db);
    if( gas == NULL )
        cerr << Error("FetchEOS(products) failed") << Exit;

	HydroState state0;        
	state0.u = 0.0;
	state0.V = solid->V_ref;
	state0.e = solid->e_ref;

    HydroState state1;
	state1.u = 0.0;
	state1.V = gas->V_ref;
	state1.e = gas->e_ref;

    double de = state1.e - state0.e;

    Hugoniot *hug0 = solid->shock(state0);
    if( hug0 == NULL )
        cerr << Error("solid->shock failed") << Exit;        
    Detonation *det = gas->detonation(state1, 0.0);
    if( det == NULL )
        cerr << Error("gas->detonation failed") << Exit;        
    
    WaveState shock1;
    WaveState CJ;
    WaveState cV;
    WaveState vN;

    cout << Center(P_form, "Ps")  << " "
         << Center(T_form, "Ts")  << " "
         << Center(P_form, "Pvb") << " "
         << Center(V_form, "Vs2") << " "
         << Center(T_form, "Ts2") << " ";
    if( deflagration )
        cout << Center(P_form, "Pdefl")  << " "
             << Center(u_form, "Ddefl")  << " ";
    
    WaveStateLabel(cout) << " "
        << Center(P_form, "Pvn")  << " "
        << Center(T_form, "Tvn")  << "\n";

	const Units *u = gas->UseUnits();
    cout << Center(P_form, u->Unit("pressure"))    << " "
         << Center(T_form, u->Unit("temperature")) << " "
         << Center(P_form, u->Unit("pressure"))    << " "
         << Center(V_form, u->Unit("V"))           << " "
         << Center(T_form, u->Unit("temperature")) << " ";
    if( deflagration )
        cout << Center(P_form, u->Unit("pressure"))    << " "
             << Center(T_form, u->Unit("velocity"))    << " ";
	WaveStateLabel(cout, *u) << " "
         << Center(P_form, u->Unit("pressure"))    << " "
         << Center(T_form, u->Unit("temperature")) << "\n";
  
    
    double dP = (P2-P1)/nsteps;
    double P = P1;
    for( int count = nsteps+1; count--; P += dP )
    {
        if( hug0->P(P,RIGHT,shock1) )
            cerr << Error("hug0->P failed") << Exit;
        cout << P_form << shock1.P         << " "
             << T_form << solid->T(shock1) << " ";
            
        Hugoniot *hug1 = solid->shock(shock1);
        if( hug1 == NULL )
            cerr << Error("hug1 is NULL failed") << Exit;        
        
	    state1.V = shock1.V;
	    state1.e = shock1.e + de;    
        double PcV = gas->P(state1);    // pressure for const volume burn
        if( !std::isnan(PcV) && PcV > shock1.P )
        {
            if( hug1->P(PcV,RIGHT,cV) )
                cerr << Error("hug1->P failed") << Exit;
            cout << P_form << PcV           << " "
                 << V_form << cV.V          << " "
                 << T_form << solid->T(cV)  << " ";
            if( deflagration )
            {
                Deflagration *deflg = gas->deflagration(cV,PcV);
                if( deflg == NULL )
                        cerr << Error("deflagration failed") << Exit;
                WaveState CJdeflg;
                if( deflg->CJwave(RIGHT,CJdeflg) )
                    cerr << Error("CJ deflagration failed") << Exit;
                cout << P_form << CJdeflg.P           << " "
                     << u_form << CJdeflg.us          << " ";
                delete deflg;
            }
        }
        else
        {
            cout << setw(P_form.width) << " " << " "
                 << setw(V_form.width) << " " << " "
                 << setw(T_form.width) << " " << " ";
        }

        if( det->Initialize(state1, shock1.P) )
        {
            cerr << Error("det->Initialize failed\n")
                << "state1 = " << state1 << ", P = " << shock1.P << "\n"
                << Exit;
        }
        det->CJwave(RIGHT,CJ);
        cout << CJ << " ";
        
        if( hug1->u_s(shock1.u+CJ.us,RIGHT,vN) )
            cerr << Error("hug1->us failed") << Exit;
        cout << P_form << vN.P             << " "
             << T_form << solid->T(vN)     << " "             
             << "\n";
        delete hug1;
    }
    delete det;
    delete hug0;
    deleteEOS(gas);
    deleteEOS(solid);
    return 0;
}
