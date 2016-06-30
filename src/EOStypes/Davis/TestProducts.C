#include <Arg.h>

#include <EOS.h>
#include <DavisProducts.h>

int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "DavisProducts";
	const char *name = "PBX9501";
	const char *units = NULL;

    EOS::Init();
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
		cerr << Error("Fetch failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;

    if( !std::isnan(V) && !isnan(e) )
    {
        if( eos->NotInDomain(V,e) )
        {
            cout << "V="   << V
                 << ", e=" << e
                 << ": not in domain\n";
        }
        else
        {
            double P = eos->P(V,e);
            cout << "P = " << P << "\n";
            double T = eos->T(V,e);
            cout << "T = " << T << "\n";
            double S = eos->S(V,e);
            cout << "S = " << S << "\n";
            double c = eos->c(V,e);
            cout << "     c  = " << c << "\n";
            eos->c2_tol = 1e-6;
            eos->c2_tol = 1e-7;
            double c2 = eos->EOS::c2(V,e);
            cout << "EOS::c  = " << sqrt(max(0.,c2)) << "\n";
            double fd = eos->FD(V,e);
            cout << "     FD = " << fd << "\n";
            eos->FD_tol = 1e-6;
            fd = eos->EOS::FD(V,e);
            cout << "EOS::FD = " << fd << "\n";
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

        double Vref = eos->V_ref;
        double eref = eos->e_ref;
        HydroState state0(Vref,eref);
        //
        cout << "\nCJ detonation state\n";
        double P0 = 1e-4;
        Detonation *det = eos->detonation(state0,P0);
        if( det == NULL )
            cerr << Error("eos->detonation failed" ) << Exit;
        WaveState CJ;
        if( det->CJwave(RIGHT,CJ) )
            cerr << Error("det->CJwave failed" ) << Exit;
        double T = eos->T(CJ.V,CJ.e);
        cout << "V e P T = " << CJ.V << ", " << CJ.e 
                     << ", " << CJ.P << ", " << T << "\n";
        cout << "D, u, c = " << CJ.us << ", " << CJ.u
                     << ", " << eos->c(CJ.V,CJ.e) << "\n";
        //
        // check isentrope
        double V0 = eos->V_ref;
        DavisProducts *DPeos =  dynamic_cast<DavisProducts *>(eos);
        if( DPeos == NULL )
    		cerr << Error("dynamic_cast failed" ) << Exit;
    	cout.setf(ios::left, ios::adjustfield);
        cout << "Reference isentrope\n";
        cout        << setw(13) << "#  V"
             << " " << setw(13) << "   e"
             << " " << setw(13) << "   T"
             << " " << setw(13) << "   S"
             << "\n";
    	cout.setf(ios::showpoint);
    	cout.setf(ios::right, ios::adjustfield);
    	cout.setf(ios::scientific, ios::floatfield);
        for( int i=1; i<=10; i++ )
        {
            double V = V0/double(i);
            double e = DPeos->eref(V);
            double T = DPeos->Tref(V);
            double S = DPeos->S(V,e);
            cout        << setw(13) << setprecision(6) << V
                 << " " << setw(13) << setprecision(6) << e
                 << " " << setw(13) << setprecision(6) << T
                 << " " << setw(13) << setprecision(6) << S
                 << "\n";
        }
        cout << "CJ isentrope\n";
        Isentrope *I = eos->isentrope(CJ);
    	cout.setf(ios::left, ios::adjustfield);
        cout        << setw(13) << "#  V"
             << " " << setw(13) << "   e"
             << " " << setw(13) << "   P"
             << " " << setw(13) << "   u"
             << " " << setw(13) << "   T"
             << " " << setw(13) << "   S"
             << "\n";
    	cout.setf(ios::right, ios::adjustfield);
        if( I == NULL )
            cerr << Error("CJ->isentrope failed") << Exit;
        WaveState wave;
        for( int i=10; i>=0; i-- )
        {
            double u = CJ.u * double(i)/10.;
            if( I->u(u,RIGHT,wave) )
                cerr << Error("I->u failed") << Exit;
            double T = eos->T(wave.V,wave.e);
            double S = eos->S(wave.V,wave.e);
            cout        << setw(13) << setprecision(6) << wave.V
                 << " " << setw(13) << setprecision(6) << wave.e
                 << " " << setw(13) << setprecision(6) << wave.P
                 << " " << setw(13) << setprecision(6) << wave.u
                 << " " << setw(13) << setprecision(6) << T
                 << " " << setw(13) << setprecision(6) << S
                 << "\n";
        }
    }
    deleteEOS(eos);
    return 0;
}
