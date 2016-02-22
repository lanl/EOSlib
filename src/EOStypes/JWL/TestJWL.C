#include <Arg.h>

#include <EOS.h>
#include <IO.h>
#include "JWL.h"

int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "JWL";
	const char *name = "HMX";
	const char *units = NULL;

    new JWL(); // force EOS::NaN to be allocated 
    double V = EOS::NaN;
    double e = EOS::NaN;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(V,V);
		GetVar(e,e);
		
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

    if( !isnan(V) && !isnan(e) )
    {
        double P = eos->P(V,e);
        cout << "P = " << P << "\n";
        double c = eos->c(V,e);
        cout << "c = " << c << "\n";
        double T = eos->T(V,e);
        cout << "T = " << T << "\n";
        double S = eos->S(V,e);
        cout << "S = " << S << "\n";
        eos->c2_tol = 1e-6;
        double c2 = eos->EOS::c2(V,e);
        cout << "EOS::c = " << sqrt(max(0.,c2)) << "\n";
        cout << "FD     = " << eos->FD(V,e) << "\n";
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
        
        JWL *jwl =  dynamic_cast<JWL *>(eos);
        if( jwl == NULL )
            cerr << Error("cast failed") << Exit;
        if( jwl->Edet > 0. )
        {
            double V0 = jwl->V0;
            double P0 = 0.0;
            double e0 = jwl->e_ref;
            HydroState state0(V0,e0,0.0);
            Detonation *det = jwl->detonation(state0,P0);
            if( det == NULL )
                cerr << Error("jwl->detonation failed") << Exit;
            
            WaveState CJ;
            if( det->CJwave(RIGHT,CJ) )
                cerr << Error(" CJwave failed") << Exit;
            double  ccj = eos->c(CJ.V, CJ.e);
            cout.precision(6);
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
                 << CJ.e - e0 - 0.5*(P0+eos->P(CJ.V, CJ.e))*(V0-CJ.V) << "\n";
    
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
    
            double V1 = V0 - 1.1*(V0 - CJ.V);
            double P1 = 1.1*CJ.P;
            double e1 = e0 + 0.5*P1*(V0-V1);
            double u1 = sqrt( (P1-P0)*(V0-V1) );
            HydroState state1(V1,e1,u1);
            Deflagration *deflg = jwl->deflagration(state1,P1);
            if( deflg == NULL )
                cerr << Error("deflagration failed") << Exit;
            WaveState CJ1;
            if( deflg->CJwave(RIGHT,CJ1) )
                cerr << Error("CJ deflagration failed") << Exit;            
            ccj = eos->c(CJ1.V, CJ1.e);
            cout.precision(6);
            cout << "\nDeflagration\n"
                 << "Pcj " << CJ1.P << " "
                 << "Dcj " << CJ1.us << " "
                 << "Vcj " << CJ1.V << " "
                 << "rho " << 1/CJ1.V << " "
                 << "ecj " << CJ1.e << " "
                 << "ucj " << CJ1.u << " "
                 << "ccj " << ccj << "\n";
            // check
            cout << "u+c - D = " << CJ1.u + ccj - CJ1.us << "\n";
            cout << "e-e1-0.5*P*dV = "
                 << CJ1.e - e1 - 0.5*(P1+eos->P(CJ1.V, CJ1.e))*(V1-CJ1.V) << "\n";
            delete deflg;
            delete det;
            // Test CJ isentrope
            cout << "\nTest CJ isentrope\n";
            Format S_form;
            T_form.width = 7;
            V_form.width = 7;
            u_form.width = 7;
            cout << Center(V_form,"V") << " "
                 << Center(e_form,"e") << " "
                 << Center(P_form,"P") << " "
                 << Center(T_form,"T") << " "
                 << Center(S_form,"S") << " "
                 << Center(P_form,"Pref") << " "
                 << Center(e_form,"eref") << " "
                 << Center(T_form,"Tref") << " "
                 << Center(S_form,"Sref") << " "
                 << "\n";
            Isentrope *Scj = eos->isentrope(CJ);
    	    if( Scj == NULL )
    		    cerr << Error("Scj is NULL") << Exit;	
        	V = CJ.V;	
            double f = pow(7*V0/V, 1./20.);
        	for( int i=0; i <20; i++, V *=f )
        	{
                double Pref = jwl->Pref(V);
                double eref = jwl->eref(V);
                double Tref = jwl->Tref(V);
                double Sref = jwl->S(V,eref);
        		WaveState w1;
        		if( Scj->V(V,RIGHT,w1) )
        			cerr << Error("Scj->V failed at V=") << V << Exit;
        		
        		cout << V_form << w1.V << " "
                     << e_form << w1.e << " "
                     << P_form << w1.P << " "
                     << T_form << jwl->EOS::T(w1) << " "
                     << S_form << jwl->EOS::S(w1) << " "
                     << P_form << Pref << " "
                     << e_form << eref << " "
                     << T_form << Tref << " "
                     << S_form << Sref << " "
                     << "\n";
        	}
        	delete Scj;
        }
    }
    deleteEOS(eos);
    return 0;
}

