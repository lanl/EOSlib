#include <Arg.h>

#include <JWL_VT.h>
#include <EOS.h>

#define NaN EOS_VT::NaN

using namespace std;

int main(int, char **argv)
{
	ProgName(*argv);
	std::string file_;
	file_ = getenv("EOSLIB_DATA_PATH");
	file_ += "/test_data/JWLTest.data";
	const char *file = file_.c_str();
	const char *type = "JWL";
	const char *name = "HMX";
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
	EOS *eos1 = FetchEOS(type,name,db);
	if( eos1 == NULL )
		cerr << Error("FetchEOS failed") << Exit;
	EOS_VT *eos = eos1->eosVT();
	if( eos == NULL )
		cerr << Error("Fetch failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;

    if( !std::isnan(V) && !std::isnan(T) )
    {
        double P = eos->P(V,T);
        cout << "P = " << P << "\n";
        double e = eos->e(V,T);
        cout << "e = " << e << "\n";
        double S = eos->S(V,T);
        cout << "S = " << S << "\n";
        double c = eos->c(V,T);
        cout << "c = " << c << "\n";
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
