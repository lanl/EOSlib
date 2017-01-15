#include <Arg.h>

#include <EOS.h>
#include <EOS_VT.h>

using namespace std;

int main(int, char **argv)
{
	ProgName(*argv);
	std::string file_;
	file_ = getenv("EOSLIB_DATA_PATH");
	file_ += "/test_data/GenHayesTest.data";
	const char *file = file_.c_str();
	const char *type = "BirchMurnaghan";
	const char *name = "HMX";
	const char *units = NULL;

    EOS::Init();
    double Ps = 60.;
    double Pmin = 1.;
    int nsteps = 10;
    int print = 0;
    
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(Ps,Ps);
		GetVar(Pmin,Pmin);
		GetVar(nsteps,nsteps);
        GetVarValue(print,print,1);
		
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
    if( print )
        eos->Print(cout);

    HydroState hstate(eos->V_ref,eos->e_ref);
    Hugoniot *H = eos->shock(hstate);
    if( H == NULL )
        cerr << Error("eos->shock failed") << Exit;
    WaveState wstate;
    if( H->P(Ps,RIGHT,wstate) )
        cerr << Error("Hugoniot failed") << Exit;
    Isentrope *I = eos->isentrope(wstate);
    if( I == NULL )
        cerr << Error("eos->isentrope failed") << Exit;
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
    InitFormat();
    WaveStateLabel(cout) << "\n";  
    int i;
    for( i=0; i<=nsteps; i++ )
    {
        double p = Ps - double(i)/double(nsteps) *(Ps - Pmin);
        if( I->P(p,LEFT,wstate) )
            cerr << Error("isentrope failed for p = ") << p << Exit;
        cout <<  wstate << "\n";       
    }
    delete(I);
    delete(H);
    deleteEOS(eos);
    return 0;
}
