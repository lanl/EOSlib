#include <Arg.h>

#include <StiffIdeal.h>

using namespace std;

int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "StiffIdeal";
	const char *name = "HMX";
	const char *units = NULL;

    EOS::Init();
    double V = EOS::NaN;
    double e = EOS::NaN;
    double lambda1 = 1;
    
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
    StiffIdeal *eosSI = dynamic_cast<StiffIdeal *>(eos);
    if( eosSI == NULL )
        cerr << Error("eosSI is NULL") << Exit;
    if( std::isnan(V) || std::isnan(e) )
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


        V = eos->V_ref;
        e = eos->e_ref;
        double P = eos->P(V,e);
        cout << "V, V1, V2 = "  << V 
                        << ", " << eosSI->V1
                        << ", " << eosSI->V2 << "\n";
        cout << "e, e1, e2 = "  << e 
                        << ", " << eosSI->e1
                        << ", " << eosSI->e2 << "\n";
        double c = eos->c(V,e);
        double c_eos = sqrt(eos->EOS::c2(V,e));
        cout << "c, c_eos = " << c << ", " << c_eos << "\n";
        return 0;        
    }
    int i;
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(7) << "lambda1"
         << " " << setw(8) << "  P"
         << " " << setw(8) << "  T"
         << " " << setw(9) << "   S"
         << " " << setw(8) << "  c"
         << " " << setw(10) << "dP/dlambda"
         << " " << setw(10) << "dT/dlambda"
         << " " << setw(8) << "  Gamma"
         << " " << setw(8) << "  CV"
         << " " << setw(8) << "  FD"
         << "\n";
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::showpoint);
    
    for( i=0; i<=10; i++)
    {
        double lambda2= double(i)/10.;
        double lambda1 = 1. - lambda2;
        eosSI->set_lambda1(lambda1);
        double P = eos->P(V,e);
        double V1 = eosSI->V1;
        double e1 = eosSI->e1;
        double V2 = eosSI->V2;
        double e2 = eosSI->e2;
        double T = eos->T(V,e);
        double S = eos->S(V,e);
        double c = eos->c(V,e);
        double dP, dT;
        eosSI->ddlambda(V,e,dP,dT);
        double Gamma = eos->Gamma(V,e);
        double CV    = eos->CV(V,e);
        double FD    = eos->FD(V,e);
        //
        // error checks
        // 
        if( std::isnan(P) )
        {
	        cout.setf(ios::fixed, ios::floatfield);
            cout <<        setw(7) << setprecision(2) << lambda1 << "\n";
	        cout.setf(ios::scientific, ios::floatfield);
            cout << "V, V1, V2 = "  << V << ", " << V1 << ", " << V2 << "\n";
            cout << "e, e1, e2 = "  << e << ", " << e1 << ", " << e2 << "\n";
            cout << "Peq, Teq = " << eosSI->Peq << ", " << eosSI->Teq << "\n";
            continue;
        }
        if( abs(V-lambda1*V1-lambda2*V2) > 1e-4*V )
            cout << "V error: " << V << " " << lambda1*V1+lambda2*V2 << "\n";
        if( abs(e-lambda1*e1-lambda2*e2) > 1e-4*e )
            cout << "e error: " << e << " " << lambda1*e1+lambda2*e2 << "\n";      
        // check PT
        HydroState PT;
        if( eosSI->PT(P,T,PT) )
            cout << "PT failed\n";
        else
        {
            if( abs(V-PT.V) > 0.001*V )
                cout << "PT.V error"  << V << ", " << PT.V << "\n";
            if( abs(e-PT.e) > 0.001*abs(e) )
                cout << "PT.e error"  << e << ", " << PT.e << "\n";                
        }
        // check Gamma and CV
        double ep = e + 0.0001;
        double Pe = eos->P(V,ep);
        double Gammap = V*(Pe-P)/(ep-e);
        if( abs(Gammap-Gamma) > 0.001*Gamma )
            cout << "Gamma error: " << Gamma << " " << Gammap << "\n";
        double Te = eos->T(V,ep);
        double CVp = (ep-e)/(Te-T);
        if( abs(CVp-CV) > 0.001*CV )
            cout << "CV error: " << CV << " " << CVp << "\n";        
        // check with d/d(lambda) with finite difference
        double lambdap = min(lambda1+0.0001,1.0);
        eosSI->set_lambda1(lambdap);
        double Pp = eos->P(V,e);
        double Tp = eosSI->Teq;
        double lambdam = max(lambda1-0.0001,0.0);
        eosSI->set_lambda1(lambdam);
        double Pm = eos->P(V,e);
        double Tm = eosSI->Teq;
        double dPfd = (Pp-Pm)/(lambdap-lambdam);
        double dTfd = (Tp-Tm)/(lambdap-lambdam);
        if( abs(dP-dPfd) > 0.001*(abs(dP)+abs(dPfd)) )
            cout << "dP error: " << dP << " " << dPfd << "\n";
        if( abs(dT-dTfd) > 0.001*(abs(dT)+abs(dTfd)) )
            cout << "dT error: " << dT << " " << dTfd << "\n";
        //
        // end of check
        // 
	    cout.setf(ios::fixed, ios::floatfield);
        cout <<        setw(7) << setprecision(2) << lambda1;
	    cout.setf(ios::scientific, ios::floatfield);
        cout << " " << setw(8) << setprecision(2) << P
             << " " << setw(8) << setprecision(2) << T
             << " " << setw(9) << setprecision(2) << S
             << " " << setw(8) << setprecision(2) << c
             << " " << setw(10) << setprecision(2) << -dP
             << " " << setw(10) << setprecision(2) << -dT
             << " " << setw(8) << setprecision(2) << Gamma
             << " " << setw(8) << setprecision(2) << CV
             << " " << setw(8) << setprecision(2) << FD
             << "\n";
        
    }
    



    deleteEOS(eos);
    return 0;
}
