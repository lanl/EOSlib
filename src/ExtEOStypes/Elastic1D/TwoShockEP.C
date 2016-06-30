#include <Arg.h>

#include "ElasticPlastic1D.h"

#define NaN EOS::NaN
//
// Compute boundary of two shock region
//
int main(int, char **argv)
{
    ProgName(*argv);
    EOS::Init();
    const char *file = "Test.data";
    const char *type = "VonMisesConstGV";
    const char *name = "HMX";
    const char *units = NULL;

    double V0 = NaN;
    double e0 = NaN;   
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        GetVar(V0,V0);
        GetVar(e0,e0);
        
        ArgError;
    }
    cout.setf(ios::showpoint);    
    cout.setf(ios::scientific,ios::floatfield);
    Format eps_form(12,4);
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
    EP->Frozen();

    if( std::isnan(V0) )
        V0 = EP->V_ref;
    if( std::isnan(e0) )
        e0 = EP->e_ref;
    double T0 = EP->T(V0,e0);
    double u0 = 0;
    HydroState state0(V0,e0,u0);
    Hugoniot *H0 = EP->shock(state0);
    if( H0 == NULL )
        cerr << Error("H0 is NULL") << Exit;

    double eps_el = 0.0;
    double eps_pl = 0.0;
    double z[1];
    z[0] = eps_pl;
    double Y = EP->yield_strength(V0,e0,z);
    double G = (3./4.)*EP->elastic->shear->dPdev_deps(V0,T0,eps_el);
    double eps_y = 0.5*Y/G;

    double V_min = (1 - 2*eps_y)*V0;
    double V_max = V0;
    double V1,e1, Yf;
    int dir = RIGHT;
    WaveState w;
    int count = 32;
    while( count-- )
    {
        V1 = 0.5*(V_min+V_max);
        if( H0->V(V1,dir,w) )
            cerr << Error("H0->V failed") << Exit;
        e1 = w.e;
        z[0] = 0.0;
        Y  = EP->yield_strength(V1,e1,z);
        Yf = EP->yield_function(V1,e1,z);
        if( Yf > Y )
            V_min = V1;
        else
            V_max = V1;      
    }
    H0->V(V_min,dir,w);
    V1 = w.V;
    e1 = w.e;
    eps_y = EP->Eps_el(V1,e1);

    WaveStateLabel(cout) << " " << Center(eps_form,"eps_y") << "\n";
    const Units *u = eos->UseUnits();
    if( u )
        WaveStateLabel(cout, *u) << "\n";
    cout << w << " " << eps_form << eps_y << "\n";
 //   
    HydroState state1(V1,e1,w.u);
    EP->Equilibrium();
    Hugoniot *H1 = EP->shock(state1);
    if( H1 == NULL )
        cerr << Error("H1 is NULL") << Exit;
    if( H1->u_s(w.us,dir,w) )
        cerr << Error("H1->u_s failed") << Exit;
    eps_y = EP->Eps_el(w.V,w.e);
    cout << w << " " << eps_form << eps_y << "\n";
    
    delete H1;
    delete H0;
    deleteEOS(eos);
    return 0;
}    
