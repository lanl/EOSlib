#include <Arg.h>

#include <EOS.h>

using namespace std;

int main(int, char **argv)
{
	ProgName(*argv);
	std::string file_;
	file_ = getenv("EOSLIB_DATA_PATH");
	file_ += "/test_data/DavisTest.data";
	const char *file = file_.c_str();
	const char *ptype = "DavisProducts";
	const char *rtype = "DavisReactants";
	const char *name = "PBX9501";
	const char *units = NULL;
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		ArgError;
	}
	if( file == NULL )
		cerr << Error("Must specify data file") << Exit;
	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;
	EOS *reactants = FetchEOS(rtype,name,db);
	if( reactants == NULL )
		cerr << Error("Fetch failed") << Exit;
    if( units && reactants->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed for reactants") << Exit;
	EOS *products = FetchEOS(ptype,name,db);
	if( products == NULL )
		cerr << Error("Fetch failed") << Exit;
    if( units && products->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed products") << Exit;

    double Vr0 = reactants->V_ref;
    double er0 = reactants->e_ref;
    HydroState stateR0(Vr0,er0);
    double P0  = reactants->P(Vr0,er0);
    cout << "P0 = " << P0 << "\n";
    double Vp0 = products->V_ref;
    double ep0 = products->e_ref;
    HydroState stateP0(Vp0,ep0);
    //
    cout << "\nCJ detonation state\n";
    Detonation *det = products->detonation(stateP0,P0);
    if( det == NULL )
        cerr << Error("products->detonation failed" ) << Exit;
    WaveState CJ;
    if( det->CJwave(RIGHT,CJ) )
        cerr << Error("det->CJwave failed" ) << Exit;
    double T = products->T(CJ.V,CJ.e);
    cout << "V e P T = " << CJ.V << ", " << CJ.e 
                 << ", " << CJ.P << ", " << T << "\n";
    cout << "D, u, c = " << CJ.us << ", " << CJ.u
                 << ", " << products->c(CJ.V,CJ.e) << "\n";
    //
    cout << "\nVN state\n";
    Hugoniot *shock = reactants->shock(stateR0);
    if( shock == NULL )
        cerr << Error("reactants->shock failed" ) << Exit;
    WaveState VN;
    if( shock->u_s(CJ.us,RIGHT,VN) )
        cerr << Error("shock->u_s failed" ) << Exit;
    T = reactants->T(VN.V,VN.e);
    cout << "V e P T = " << VN.V << ", " << VN.e 
                 << ", " << VN.P << ", " << T
                 << "\n";
    cout << "u, c = "  << VN.u
               << ", " << reactants->c(VN.V,VN.e)
               << "\n";
    cout << "CV = " << reactants->CV(VN.V,VN.e) << "\n";
    delete det;
    delete shock;
    deleteEOS(reactants);
    deleteEOS(products);
    return 0;
}
