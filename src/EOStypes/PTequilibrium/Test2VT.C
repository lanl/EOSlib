#include <Arg.h>

#include <PTequilibrium_VT.h>
#include <EOS.h>

using namespace std;

//
// need to set environment variable SharedLibDirEOS
// cd ../..; . SetEnv
//
#define NaN EOS_VT::NaN
int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "PTequilibrium";
	const char *name = "mix0.8";
	const char *units = NULL;

    EOS_VT::Init();
    double V = NaN;
    double T = NaN;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(V,V);
		GetVar(T,T);
		
		ArgError;
	}

	if( file == NULL )
		cerr << Error("Must specify data file") << Exit;

	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;

	EOS_VT *eos = FetchEOS_VT(type,name,db);
    if( eos == NULL )
    {
    	EOS *eos1 = FetchEOS(type,name,db);
    	if( eos1 == NULL )
    		cerr << Error("FetchEOS failed") << Exit;
    	eos = eos1->eosVT();
    	if( eos == NULL )
    		cerr << Error("Fetch failed") << Exit;
    }
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;

    if( !std::isnan(V) && !std::isnan(T) )
    {
        PTequilibrium_VT *eosPT = dynamic_cast<PTequilibrium_VT*>(eos);
        if( eosPT == NULL )
            cerr << Error("eosPT is NULL") << Exit;
        double P = eos->P(V,T);
        if( std::isnan(P) )
            cerr << Error("eos->P failed") << Exit;
        double V1 = eosPT->V1;
        double V2 = eosPT->V2;       
        cout << "P = " << P
             << ", V1 = " << V1
             << ", V2 = " << V2
             << "\n";
        double e = eos->e(V,T);
        cout << "e = " << e 
             << ", e1 = " << eosPT->eos1->e(V1,T)
             << ", e2 = " << eosPT->eos2->e(V2,T)
             << "\n";
        double S = eos->S(V,T);
        cout << "S = " << S
             << ", S1 = " << eosPT->eos1->S(V1,T)
             << ", S2 = " << eosPT->eos2->S(V2,T)
             << "\n";
        double c = eos->c(V,T);
        cout << "c = " << c
             << ", c1 = " << eosPT->eos1->c(V1,T)
             << ", c2 = " << eosPT->eos2->c(V2,T)
             << "\n";
    }
    else    
    {
        const char *info = db.BaseInfo("EOS_VT");
        if( info )
        {
            cout << "Base info\n" << info << "\n";
            info = db.TypeInfo("EOS_VT",type);
            if( info )
                cout << "Type info\n" << info << "\n";
        }
	    eos->PrintAll(cout);
        /****
        EOS *Ve = eos->eosVe();
        if( Ve == NULL )
    		cerr << Error("Ve is NULL" ) << Exit;
        Ve->Print(cout);
        deleteEOS(Ve);
        ***/
    }

    deleteEOS_VT(eos);
    return 0;
}
