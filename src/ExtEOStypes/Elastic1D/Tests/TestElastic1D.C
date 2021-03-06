#include <Arg.h>

#include <EOS.h>
#include <EOS_VT.h>
#include "Elastic1D.h"
using namespace std;

int main(int, char **argv)
{
	ProgName(*argv);
	//const char *file = "Test.data";
	std::string file_;
        file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
	file_ += "/test_data/Elastic1DTest.data";
        const char * file = file_.c_str();
	const char *type = "ElasticConstGV";
	const char *name = "HMX";
	const char *units = NULL;

    EOS::Init();
    double V = EOS::NaN;
    double e = EOS::NaN;
    double eps_el = 0.0;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(type,type);
		GetVar(units,units);
        
		GetVar(V,V);
		GetVar(e,e);
		GetVar(eps,eps_el);
		
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

    if( !std::isnan(V) && !std::isnan(e) )
    {
        if( eos->NotInDomain(V,e) )
        {
            cout << "V="   << V
                 << ", e=" << e
                 << ": not in domain\n";
        }
        else
        {
            Elastic1D *Eeos = dynamic_cast<Elastic1D*>(eos);
            if( Eeos == NULL )
                cerr << Error("Eeos is NULL") << Exit;
            Eeos->eps_el_ref = eps_el;
            double P = eos->P(V,e);
            cout << "P = " << P << "\n";
            double T = eos->T(V,e);
            cout << "T = " << T << "\n";
            double S = eos->S(V,e);
            cout << "S = " << S << "\n";
            double c = eos->c(V,e);
            cout << "c = " << c << "\n";
            eos->c2_tol = 1e-6;
            double c2 = eos->EOS::c2(V,e);
            cout << "EOS::c = " << sqrt(max(0.,c2)) << "\n";
            eos->FD_tol = 1e-6;
            double fd = eos->FD(V,e);
            cout << "FD = " << fd << "\n";
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
	    eos->PrintAll(cout);
        
        EOS_VT *VT = eos->eosVT();
        if( VT == NULL )
    		cerr << Error("VT is NULL" ) << Exit;
        VT->PrintAll(cout);
        deleteEOS_VT(VT);
    }

    deleteEOS(eos);
    return 0;
}
