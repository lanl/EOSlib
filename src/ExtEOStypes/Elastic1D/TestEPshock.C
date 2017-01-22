#include <Arg.h>

#include "ElasticPlastic1D.h"
#include <IO.h>

#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    EOS::Init();
    //const char *file = "Test.data";
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/Elastic1DTest.data";
    const char * file = file_.c_str();
    
    const char *type = "VonMisesConstGV";
    const char *name = "HMX";
    const char *units = NULL;
	int nsteps = 10;
    int frozen = 1;
    double umax = NaN;
    double V0 = NaN;
    double e0 = NaN;   
    double u0 = 0.0;   
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);     
        GetVar(umax,umax);     
        GetVar(nsteps,nsteps);
        GetVarValue(frozen,frozen,1);
        GetVarValue(equilibrium,frozen,0);
        GetVar(V0,V0);
        GetVar(e0,e0);
        GetVar(u0,u0);
        ArgError;
    }
	cout.setf(ios::showpoint);    
    cout.setf(ios::scientific,ios::floatfield);
    Format eps_form(12,4);
    Format S_form(12,4);
    V_form.width = 12;
    e_form.width = 12;
    u_form.width = 12;
    P_form.width = 12;
    T_form.width = 12;
    
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    ElasticPlastic1D *EP = dynamic_cast<ElasticPlastic1D *>(eos);
    if( EP == NULL )
        cerr << Error("dynamic_cast failed") << Exit;

    if( std::isnan(V0) )
        V0 = EP->V_ref;
	if( std::isnan(e0) )
        e0 = EP->e_ref;
    double c0 = EP->c(V0,e0);
	HydroState state0(V0,e0,u0);
    int dir = RIGHT;

    int i;
	double Nsteps = (double) nsteps;
    if( std::isnan(umax) )
        umax = 0.5*c0;
    
    if( frozen )
        EP->Frozen();
    else
        EP->Equilibrium();
	Hugoniot *H1 = EP->shock(state0);
	if( H1 == NULL )
		cerr << Error("H1 is NULL") << Exit;
	cout << "Test "
         << (frozen ? "frozen" : "equilibrium")
         << " Hugoniot\n";	
	WaveStateLabel(cout) << " " << Center(u_form, "c")
                         << " " << Center(T_form, "T")
                         << " " << Center(S_form, "S")
                         << " " << Center(eps_form, "eps_el")
                         << " " << Center(P_form, "Pdev")
                         << "\n";
	const Units *u = eos->UseUnits();
	if( u )
	    WaveStateLabel(cout, *u) << " "
	                             << Center(u_form,u->Unit("velocity")) << " "
	                             << Center(T_form,u->Unit("temperature")) << " "
	                             << Center(S_form,u->Unit("specific_heat")) << " "
	                             << Center(eps_form,"") << " "
	                             << Center(P_form,u->Unit("pressure")) << " "
                                 << "\n";

    for(i=0; i<=nsteps; i++ )
	{
        double u = u0 +(i/Nsteps)*(umax-u0);
		WaveState w1;
		if( H1->u(u,dir,w1) )
			cerr << Error("H1->u failed") << Exit;
        double eps = EP->Eps_el(w1.V,w1.e);
        if( eps > 0 )
        { // debug error
            cout << "V=" << w1.V << ", e=" << w1.e << "\n";
            double *z = EP->z_f(w1.V,w1.e);
            cout << "eps_pl = " << z[0] << "\n";
            cout << "eps_el = " << eps << "\n";
        }
        double Pdev = EP->elastic->Pdev(w1.V,w1.e,eps);
        double c2   = EP->elastic->c2(w1.V,w1.e,eps);   // frozen sound speed
        if( c2 < 0 )
            cerr << Error("c2 < 0") << Exit;
        double c = sqrt(c2);
		cout << w1
             << " " <<   u_form << c 
             << " " <<   T_form << EP->EOS::T(w1) 
             << " " <<   S_form << EP->EOS::S(w1) 
             << " " << eps_form << eps 
             << " " <<   P_form << Pdev
             << "\n";
	}
	delete H1;

    deleteEOS(eos);
    return 0;
}
