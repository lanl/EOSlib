#include <Arg.h>
#include <LocalMath.h>
#include <EOS.h>

int main(int, char **argv)
{
    const char *files    = "HE.data";    
    const char *type     = NULL;
    const char *name     = NULL;
    const char *material = "BirchMurnaghan::PBX9501.reactant";
    const char *units    = "hydro::std";
    const char *lib      = "../lib/Linux";

    double Pmin =  5.;  // min P for shock
    double Pmax = 35.;  // max P for shock
    int nsteps  = 30;   // number of steps on Hugoniot locus


while(*++argv)
    {
        GetVar(file,files);
        GetVar(files,files);
        GetVar(type,type);
        GetVar(name,name);
        GetVar(material,material);
        GetVar(units,units);
        GetVar(lib,lib);

        GetVar(Pmin,Pmin);
        GetVar(Pmax,Pmax);

        ArgError;
    }
    if( material )
    {
        if( type || name )
            cerr << Error("Can not specify both material and name or type")
                 << Exit;
        if( TypeName(material,type,name) )
            cerr << Error("syntax error, material = ") << material
                 << Exit;
    }
    else if( type==NULL || name==NULL )
    {
        cerr << Error("Specify either material or name and type")
             << Exit;
    }
// fetch eos
    DataBase db;
    if( db.Read(files) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("material not found, ") << type << "::" << name << Exit;    
    if( eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
//  setup Hugoniot
    double V0 = eos->V_ref; 
    double e0 = eos->e_ref;
    double P0 = eos->P(V0,e0);
    double c0 = eos->c(V0,e0);
    double S0 = eos->S(V0,e0);
    HydroState state0(V0,e0);
    Hugoniot *hug = eos->shock(state0);
    if( hug==NULL )
        cerr << Error("eos->shock failed" ) << Exit;
//  Print header    
    Format S_form(9,2);
    V_form.width     = 7;
    V_form.precision = 4;
    V_form.fixed = 1;
    e_form.width     = 7;
    e_form.precision = 4;
    e_form.fixed = 1;
    u_form.width     = 7;
    u_form.precision = 4;
    u_form.fixed = 1;
    us_form.width     = 7;
    us_form.precision = 4;
    us_form.fixed = 1;
    //
    P_form.width     = 5;
    P_form.precision = 0;
    P_form.fixed = 1;
    /****
    P_form.width     = 8;
    P_form.precision = 2;
    P_form.fixed = 0;
    ****/
    T_form.width     = 6;
    T_form.precision = 0;
    T_form.fixed = 1;
    //
    WaveStateLabel(cout)
          << " " << Center(T_form, "T")
          << " " << Center(S_form, "dS")
           << "\n";
    const Units *u = eos->UseUnits();
    if( u )
    {
        WaveStateLabel(cout, *u)
          << " " << Center(T_form, u->Unit("T"))
          << " " << Center(S_form, u->Unit("S"))
          << "\n";
    }    
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
//
    double dP = (Pmax-Pmin)/double(nsteps);
    int i;
    for( i=0; i<=nsteps; i++ )
    {
        double P = Pmin + double(i)*dP;
        WaveState wstate;
        if( hug->P(P,RIGHT,wstate) )
            cerr << Error("hug->P failed") << Exit;
        double V = wstate.V;
        double e = wstate.e;
        double T = eos->T(V,e);
        double S = eos->S(V,e);
        cout << wstate
             << " " << T_form << T
             << " " << S_form << S-S0
             << "\n";
    }
    delete hug;
    deleteEOS(eos);
    return 0;
}
