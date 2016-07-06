#include <Arg.h>

#include <EOS.h>
#include "HEfit.h"

using namespace std;

int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "HEfit";
	const char *name = "PBX9501";
	const char *units = NULL;

    new HEfit(); // force EOS::NaN to be allocated 
    double V  = EOS::NaN;
    double e  = EOS::NaN;
    double P0 = EOS::NaN;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(V,V);
		GetVar(e,e);
		GetVar(P0,P0);
		
		ArgError;
	}

	if( file == NULL )
		cerr << Error("Must specify data file") << Exit;

	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;

	EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
	{
		cerr << "eos == NULL\n";
		cerr << Error("Fetch failed") << Exit;
	}
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;

    HEfit *gas =  dynamic_cast<HEfit *>(eos);
    if( gas == NULL )
        cerr << Error("cast failed") << Exit;

    if( !std::isnan(V) && !std::isnan(e) )
    {
        double P = eos->P(V,e);
        cout << "P = " << P << "\n";
        double c = eos->c(V,e);
        cout << "c = " << c << "\n";
        eos->c2_tol = 1e-6;
        double c2 = eos->EOS::c2(V,e);
        cout << "EOS::c = " << sqrt(max(0.,c2)) << "\n";
        cout << eos->FD(V,e) << "\n";

        if( !std::isnan(P0) )
        {
            HydroState state0(V,e,0.0);
            Detonation *det = gas->detonation(state0,P0);
            if( det == NULL )
                cerr << Error("gas->detonation failed") << Exit;
            WaveState CJ;
            det->CJwave(RIGHT,CJ);
            double  ccj = eos->c(CJ.V, CJ.e);
            cout << "Pcj " << CJ.P << " "
                 << "Dcj " << CJ.us << " "
                 << "Vcj " << CJ.V << " "
                 << "rho " << 1/CJ.V << " "
                 << "ecj " << CJ.e << " "
                 << "ucj " << CJ.u << " "
                 << "ccj " << ccj << "\n";
            // check
            cout << "u+c - D = " << CJ.u + ccj - CJ.us << "\n";
            cout << "e-e0-0.5*P*dV = "
                 << CJ.e - e - 0.5*(P0+eos->P(CJ.V, CJ.e))*(V-CJ.V) << "\n";
        }
    }
    else    
    {
        const char *info = db.BaseInfo("EOS");
        if( info == NULL )
    		cerr << Error("No EOS info" ) << Exit;
        cout << "Base info\n"
             << info << "\n";

        info = db.TypeInfo("EOS",type);
        if( info == NULL )
    		cerr << Error("No type info" ) << Exit;
        cout << "Type info\n"
             << info << "\n";
        
	    eos->Print(cout);
        
        double V0 = gas->V_ref;
        double e0 = gas->e_ref;
        HydroState state0(V0,e0,0.0);
        Detonation *det = gas->detonation(state0,0.0);
        if( det == NULL )
            cerr << Error("gas->detonation failed") << Exit;
        
        WaveState CJ;
        det->CJwave(RIGHT,CJ);
        double  ccj = eos->c(CJ.V, CJ.e);
        cout << "Pcj " << CJ.P << " "
             << "Dcj " << CJ.us << " "
             << "Vcj " << CJ.V << " "
             << "rho " << 1/CJ.V << " "
             << "ecj " << CJ.e << " "
             << "ucj " << CJ.u << " "
             << "ccj " << ccj << "\n";
        // check
        cout << "u+c - D = " << CJ.u + ccj - CJ.us << "\n";
        cout << "e-e0-0.5*P*dV = "
             << CJ.e - e0 - 0.5*eos->P(CJ.V, CJ.e)*(V0-CJ.V) << "\n";


        cout << "    ";
        WaveStateLabel(cout) << "\n";
        WaveState wave;

        double P = 1.1*CJ.P;
        if( det->P(P,RIGHT,wave) )
            cerr << Error("det->P failed") << Exit;
        cout << "P " << wave << "\n";

        double V = wave.V; wave.P = 0;
        if( det->V(V,RIGHT,wave) )
            cerr << Error("det->V failed") << Exit;
        cout << "V " << wave << "\n";
        
        double D = wave.us; wave.P = 0;
        if( det->u_s(D,RIGHT,wave) )
            cerr << Error("det->u_s failed") << Exit;
        cout << "D " << wave << "\n";
        
        double u = wave.u; wave.P = 0;
        if( det->u(u,RIGHT,wave) )
            cerr << Error("det->u failed") << Exit;
        cout << "u " << wave << "\n";
            
        delete det;
    }
    deleteEOS(eos);

    return 0;
}

