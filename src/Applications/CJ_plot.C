#include <Arg.h>
#include <EOS.h>

int main(int, char **argv)
{
	ProgName(*argv);
    InitFormat();

	const char *files    = "EOS.data";    
    const char *reactant = "BirchMurnaghan::HMXfit";
    const char *products = "HEfit::PBX9501";

    double P1 = 0;
    double P2 = 5;
    int nsteps = 10;
    
    while(*++argv)
	{
	    GetVar(file,files);
	    GetVar(files,files);
	    GetVar(reactant,reactant);
	    GetVar(products,products);

        GetVar(P1,P1);
        GetVar(P2,P2);
        GetVar(nsteps,nsteps);
     
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
    WaveState vN;

  
    
    double dP = (P2-P1)/nsteps;
    double P = P1;
    for( int count = nsteps+1; count--; P += dP )
    {
        if( hug0->P(P,RIGHT,shock1) )
            cerr << Error("hug0->P failed") << Exit;
        cout << V_form << shock1.V << " "
             << P_form << shock1.P << " ";
            
        Hugoniot *hug1 = solid->shock(shock1);
        if( hug1 == NULL )
            cerr << Error("hug1 is NULL failed") << Exit;        
        
	    state1.V = shock1.V;
	    state1.e = shock1.e + de;
        if( det->Initialize(state1, shock1.P) )
        {
            cerr << Error("det->Initialize failed\n")
                << "state1 = " << state1 << ", P = " << shock1.P << "\n"
                << Exit;
        }
        det->CJwave(RIGHT,CJ);
        cout << V_form << CJ.V << " "
             << P_form << CJ.P << " ";
        if( hug1->u_s(shock1.u + CJ.us,RIGHT,vN) )
            cerr << Error("hug1->us failed") << Exit;
        cout << V_form << vN.V << " "
             << P_form << vN.P << "\n";
        delete hug1;
    }
    delete det;
    delete hug0;
    deleteEOS(gas);
    deleteEOS(solid);
    return 0;
}
