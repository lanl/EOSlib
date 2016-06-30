#include <Arg.h>

#include <PTequilibrium.h>
#include <EOS_VT.h>

int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "PTequilibrium";
	const char *name = "mix0.8";
	const char *units = NULL;

    EOS::Init();
    double V = EOS::NaN;
    double e = EOS::NaN;
    //
    // debug
    V = 4.6796e+01;
    e = 1.3508e+00;
    //
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
        if( 0 && eos->NotInDomain(V,e) )
        {
            cout << "V="   << V
                 << ", e=" << e
                 << ": not in domain\n";
        }
        else
        {
            PTequilibrium *eosPT = dynamic_cast<PTequilibrium *>(eos);
            if( eosPT == NULL )
                cerr << Error("eosPT is NULL") << Exit;          
            double P = eos->P(V,e);
            double V1 = eosPT->V1;
            double e1 = eosPT->e1;
            double V2 = eosPT->V2;       
            double e2 = eosPT->e2;
            double Vav = eosPT->lambda1*V1 + eosPT->lambda2*V2;
            double eav = eosPT->lambda1*e1 + eosPT->lambda2*e2;
            double P1 = eosPT->eos1->P(V1,e1);
            double P2 = eosPT->eos2->P(V2,e2);
            cout << "Vav = "  << Vav
                 << ", V1 = " << V1
                 << ", V2 = " << V2
                 << "   count " << eosPT->count
                 << "\n";
            cout << "eav = "  << eav
                 << ", e1 = " << e1
                 << ", e2 = " << e2
                 << "\n";
            cout << "P = " << P
                 << ", P1 = " << P1
                 << ", P2 = " << P2
                 << "\n";
            double T1 = eosPT->eos1->T(V1,e1);
            double T2 = eosPT->eos2->T(V2,e2);
            double T = eos->T(V,e);
            cout << "T = " << T
                 << ", T1 = " << T1
                 << ", T2 = " << T2
                 << "\n";
            if( std::isnan(P) )
                cerr << Error("failed") << Exit;
            double S = eos->S(V,e);
            cout << "S = " << S << "\n";
            double c1 = eosPT->eos1->c(V1,e1);
            double c2 = eosPT->eos2->c(V2,e2);
            double c = eos->c(V,e);
            cout << "c = " << c
                 << ", c1 = " << c1
                 << ", c2 = " << c2
                 << "\n";
            eos->c2_tol = 1e-4;
            c2 = eos->EOS::c2(V,e);
            cout << "EOS::c = " << sqrt(max(0.,c2)) << "\n";
            double fd = eos->FD(V,e);
            cout << "FD = " << fd << "\n";
            //eos->FD_tol = 1e-6;
            //fd = eos->EOS::FD(V,e);
            //cout << "EOS::FD = " << fd << "\n";
            double cT2 = V*eos->KT(V,e);
            double cT = (cT2>0) ? sqrt(cT2) : 0.0;
            cout << "cT = " << cT << "\n";
            // test d/dlambda
            double dP, dT;
            if( eosPT->ddlambda(V,e,dP,dT) )
                cerr << Error("ddlambda failed") << Exit;
            cout << "dP, dT = " << dP << ", " << dT << "\n";
            P = eosPT->P(V,e);
            T = eosPT->T(V,e);
            double dlambda = 0.0001;
            if( eosPT->lambda1+dlambda >= 1 )
                dlambda = -dlambda;
            eosPT->set_lambda1(eosPT->lambda1+dlambda);
            P1 = eosPT->P(V,e);
            T1 = eosPT->T(V,e);
            dP = (P1-P)/dlambda;
            dT = (T1-T)/dlambda;
            cout << "       = " << dP << ", " << dT << "\n";           
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
        
	    eos->PrintAll(cout);
        /***
        EOS_VT *VT = eos->eosVT();
        if( VT == NULL )
    		cerr << Error("VT is NULL" ) << Exit;
        VT->Print(cout);
        deleteEOS_VT(VT);
        ***/
    }

    deleteEOS(eos);
    return 0;
}
