#include <Arg.h>
#include <LocalMath.h>
#include <ExtEOS.h>
#include <ShockPolar_gen.h>

const char *help[] = {    // list of commands printed out by help option
    "locus of shock polar / Prandtl-Meyer fan",
    "material:",
    "  name        name    # material name",
    "  type        name    # EOS type",
    "  material    name    # type::name",
    "  file[s]     file    # : separated list of data files [EOS.data]",
    "  units       name    # default units from data base [hydro::std]",
     "",
    "nsteps        value    # number of points on detonation locus",
    "Pmax          value    # max P on polar",
    0
};
void Help(int status)
{
    const char **list;
    for(list=help ;*list; list++)
    {
        cerr << *list << "\n";
    }
    exit(status);
}
//
//
int main(int, char **argv)
{
    ProgName(*argv);
    EOS::Init();
    InitFormat();
    // material
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ApplicationsEOS.data";
    const char * files = file_.c_str();
    //const char *files    = "EOS.data";    
    const char *type     = NULL;
    const char *name     = NULL;
    const char *material = "ArrheniusHE::PBX9501";//NULL;
    const char *units    = "hydro::std";
    int nsteps = 10;
    int dir = RIGHT;
    double Pmax = 0.0;
// process command line arguments
//    if( argv[1] == NULL )
//        Help(-1);
    while(*++argv)
    {
        // material
        GetVar(file,files);
        GetVar(files,files);
        GetVar(type,type);
        GetVar(name,name);
        GetVar(material,material);
        GetVar(units,units);
        GetVar(nsteps,nsteps);
        GetVar(Pmax,Pmax);
        // help
        if( !strcmp(*argv, "?") || !strcmp(*argv,"help") )
            Help(0);
        ArgError;
    }
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
    Format M_form;
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
// Fetch eos
    DataBase db;
    if( db.Read(files) )
    	cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("material not found, ") << type << "::" << name << Exit;
    if( eos->ConvertUnits(units, db) )
    	cerr << Error("ConvertUnits failed") << Exit;
    ExtEOS *HE = dynamic_cast<ExtEOS *>(eos);
    if( HE == NULL )
        cerr << Error("eos not ExtEOS") << Exit;
    IDOF *zref =HE->Zref();
    double *z = &(*zref);
    int i = zref->var_index("lambda");
    if( i < 0 )
        cerr << Error("eos not HE") << Exit;
// ref state
    HydroState state0(eos->V_ref,eos->e_ref);
    double P0 = eos->P(state0);
    Detonation *det =  HE->detonation(state0,P0);
    if( det == NULL )
        cerr << Error("det is NULL") << Exit;   
    WaveState CJ;
    if( det->CJwave(dir,CJ) )
        cerr << Error("CJwave failed") << Exit;
    double Pcj = CJ.P;
    if( Pmax <= Pcj )
        Pmax = 1.1*Pcj;
    WaveState wstate;
    if( det->P(Pmax,dir,wstate) )
        cerr << Error("det->Pmax failed") << Exit;
    double q0 = wstate.us;
    z[i] = 1.0;

    double rad2deg = 45./atan(1.);

    //WaveStateLabel(cout) << "\n";
    //WaveStateLabel(cout, *eos->UseUnits()) << "\n";
    cout << ".line\n";

    PolarState pstate0;
    double P_sonic = 0.0;
    double theta_sonic  = 0.0;
    int j;
    for( j=0; j<=nsteps; j++ )
    {
        double p = Pcj + double(j)/double(nsteps) * (Pmax-Pcj);
        if( det->P(p,dir,wstate) )
            cerr << Error("det->P failed") << Exit;
        double c = HE->c(wstate.V,wstate.e,z);
        double us = wstate.us;
        double up = wstate.u;
        double qpar = sqrt(q0*q0-us*us);
        double dtheta = atan(qpar*up/(q0*q0-us*up));
        double u = us-up; u = sqrt(qpar*qpar+u*u);
        /**
        cout << wstate
             << " " << c
             << " " << u-c
             << " " << dtheta
             << "\n";
        **/
        cout << rad2deg*dtheta << " " << p << "\n";
        if( j == 0 )
        {
            pstate0.V = wstate.V;
            pstate0.e = wstate.e;
            pstate0.q = u;
            pstate0.theta = dtheta;
        }
        if( P_sonic == 0.0 && u < c )
        {
            P_sonic = p;
            theta_sonic = dtheta;
        }
    }
    delete det;
    cout << ".points\n";
    cout << rad2deg*theta_sonic << " " << P_sonic << "\n";

    ShockPolar_gen Polar(*eos);
    if( Polar.Initialize(pstate0) )
        cerr << Error("Polar.Initialize(pstate0) failed") << Exit;
    double P1 = Polar.P0;
    double P2 = Polar.Pmax.P;   
    cout << ".line\n";
    for( j=0; j<=nsteps; j++ )
    {
        double p = P1 + double(j)/double(nsteps)*(P2-P1);
        PolarWaveState pwave;
        if( Polar.P(p,LEFT,pwave) )
            cerr << Error("Polar.P failed") << Exit;
        cout << rad2deg*pwave.theta << " " << p << "\n";
    }   
    deleteEOS(eos);    
    return 0;
}
