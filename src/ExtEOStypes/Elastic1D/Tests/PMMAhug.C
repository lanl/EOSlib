#include <Arg.h>

#include "ElasticPlastic1D.h"
#include "Polymer.h"
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
    
    const char *type = "Polymer";
    const char *name = "PMMA";
    const char *units = NULL;
	int nsteps = 30;
    double etamin = 0.0;
    double etamax = 0.3;
    double V0 = NaN;
    double e0 = NaN;   
    double u0 = 0.0;   
    int frozen = 0;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);     
        GetVar(eta,etamax);     
        GetVar(etamin,etamin);     
        GetVar(nsteps,nsteps);
        GetVar(V0,V0);
        GetVar(e0,e0);
        GetVar(u0,u0);
        GetVarValue(frozen,frozen,1);
        GetVarValue(equilibrium,frozen,0);
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
    Polymer *EP = dynamic_cast<Polymer *>(eos);
    if( EP == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    PolyG *shear = EP->Pshear;

    if( std::isnan(V0) )
        V0 = EP->V_ref;
	if( std::isnan(e0) )
        e0 = EP->e_ref;
    double c0 = EP->c(V0,e0);
	HydroState state0(V0,e0,u0);
    int dir = RIGHT;

    int i;
	double Nsteps = (double) nsteps;
    
    if( frozen )
        EP->Frozen();
    else
        EP->Equilibrium();
	Hugoniot *H1 = EP->shock(state0);
	if( H1 == NULL )
		cerr << Error("H1 is NULL") << Exit;
    cout << Center(eps_form,"eta") << " ";
	WaveStateLabel(cout) << " " << Center(u_form, "clong")
                         << " " << Center(u_form, "ctran")
                         << " " << Center(T_form, "T")
                         << " " << Center(eps_form, "eps_el")
                         << " " << Center(P_form, "Y")
                         << "\n";
	const Units *u = eos->UseUnits();
	if( u )
    {
        cout << Center(eps_form,"") << " ";
	    WaveStateLabel(cout, *u) << " " << Center(u_form,u->Unit("velocity"))
	                             << " " << Center(u_form,u->Unit("velocity"))
                                 << " " << Center(T_form,u->Unit("temperature"))
                                 << " " << Center(eps_form,"")
                                 << " " << Center(P_form,"pressure")
                                 << "\n";
    }

    for(i=0; i<=nsteps; i++ )
	{
        double eta = etamin + (i/Nsteps)*(etamax-etamin);   // eta = 1 - V/V0
        double V = V0*(1-eta);
		WaveState w1;
		if( H1->V(V,dir,w1) )
			cerr << Error("H1->V failed") << Exit;
        double eps_el = EP->Eps_el(w1.V,w1.e);
        double eps_pl = log(1.-eta)-eps_el;
        double c2   = EP->elastic->c2(w1.V,w1.e,eps_el); // frozen sound speed
        if( c2 < 0. )
            cerr << Error("c2 < 0") << Exit;
        double clong = sqrt(c2);
        c2  = shear->cs2(V,eps_el);
        double ctran = sqrt(c2);
        double Y = EP->yield_strength(V);
		cout << eps_form << eta
             << " " <<  w1
             << " " <<   u_form << clong 
             << " " <<   u_form << ctran 
             << " " <<   T_form << EP->EOS::T(w1) 
             << " " << eps_form << eps_el
             << " " <<   P_form << Y
             << "\n";
	}
	delete H1;

    deleteEOS(eos);
    return 0;
}
