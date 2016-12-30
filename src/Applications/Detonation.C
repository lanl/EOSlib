#include <Arg.h>
#include <ExtEOS.h>
// test
#include <HEburn.h>
#include <EOS_VT.h>
//

#define NaN EOS::NaN

EOS *eos    = NULL;
ExtEOS *HE  = NULL;
Format FD_form;


void PrintState(WaveState &state, const char *str=NULL)
{
    double c  = sqrt(eos->c2(state.V,state.e));
    double FD = eos->FD(state.V,state.e);
    double T  = eos->T(state.V,state.e);
    double rate;

    cout << state   << " "
         << c_form  << c  << " "
         << FD_form << FD << " "
         << T_form  << T ;
    if( str ) cout << "  " << str;
    cout << "\n";
}

void PrintLine()
{
    cout.fill('-');
    cout << setw(V_form.width)  << "" << "-"
         << setw(e_form.width)  << "" << "-"
         << setw(u_form.width)  << "" << "-"
         << setw(P_form.width)  << "" << "-"
         << setw(us_form.width) << "" << "-"
         << setw(c_form.width)  << "" << "-"
         << setw(FD_form.width) << "" << "-"
         << setw(T_form.width)  << "" << "\n";
    cout.fill(' ');
}

const char *help[] = {    // list of commands printed out by help option
    "name        name    # material name",
    "type        name    # EOS type",
    "material    name    # type::name",
    "units       name    # units for Detonation",
    "file[s]     file    # colon separated list of data files [EOS.data]",
    "lib         name    # directory for EOSlib shared libraries",
    "                    # default environ variable EOSLIB_SHARED_LIBRARY_PATH",
    "ref state: (default, material ref state)",
    "V0     value   # specific volume of reference state",
    "e0     value   # specific energy of reference state",
    "state  V0 e0   # reference state",
    "u0     value   # velocity of reference state",
    "P0     value   # non-equilibrium pressure for detonation locus",
    "",
    "initial state: (default, ref state)",
	"InitShock      # Initial state on shock locus based on ref state",
	"InitIsentrope  # Initial state on isentrope from ref state",
    "init_left      # left  facing wave for initial state",
    "init_right     # right facing wave for initial state (default)",
    "CJleft         # left  facing wave for CJ state",
    "CJright        # right facing wave for CJ state (default)",
    "u_p    value   # shock particle velocity",
    "u_s    value   # shock velocity",
    "Ps     value   # shock pressure",
    "Vs     value   # shock specific volume",
    "",
    "Detonation locus:",
    "lambda  value  # partially burned locus (first reaction)",
    "lambda2 value  # partially burned locus (second reaction)",
    "left           # left facing wave",
    "right          # right facing wave (default)",
    "weak           # weak detonation, rather than release isentrope",
    "Pdet value     # release isentrope from detonation state",
    "release        # release from CJ state",
    "",
    "P              # wave curve by P",
    "u              # wave curve by u",
    "us             # wave curve by us",
    "V              # wave curve by V",
    "(default)      # CJ state only",
    "",
    "range   var1 var2    # range of wave curve",
    "var1    value        # default is CJ state",
    "var2    value",
    "",
    "P2    value    # P  & var2=value",
    "u2    value    # u  & var2=value",
    "us2   value    # us & var2=value",
    "V2    value    # V  & var2=value",
    "",
    "nsteps    value    # number of points on detonation locus",
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

namespace VAR
{
    enum type { none, P, Up, Us, V };
}
namespace WAVE
{
    enum type { state, shock, isentrope };
}

int main(int, char **argv)
{
    ProgName(*argv);
    EOS::Init();          // sets EOS::NaN
    InitFormat();

    const char *files    = NULL;
    const char *lib      = NULL;

    const char *type     = NULL;
    const char *name     = NULL;
    const char *material = NULL;
    const char *units    = "hydro::std";
    const char *EOSlog   = "EOSlog";      // EOS error log file

    int nsteps = 10;

    double V0 = NaN;
    double e0 = NaN;
    double u0 = NaN;
    double P0 = NaN;
    double T0 = NaN;

    WAVE::type InitLocus = WAVE::shock;
    int  InitDir         = RIGHT;	
    int  CJdir           = RIGHT;
    VAR::type  InitVar   = VAR::none;
    double var0          = NaN;    // value on init state locus
    double lambda        = 1.;     // partially burned locus, first  reaction
    double lambda2       = 2.;     // partially burned locus, second reaction
    int weak             = 0;      // flag, weak detonation if P < Pcj
    int release          = 0;      // flag, isentrope from CJ
    double Pdet          = NaN;    // isentrope from Pdet

    int dir        = RIGHT;
    VAR::type loop = VAR::none;
    double var1    = NaN;
    double var2    = NaN;
// process command line arguments
//    if( argv[1] == NULL )
//        Help(-1);
    while(*++argv)
    {
        GetVar(file,files);
        GetVar(files,files);
        GetVar(lib,lib);

        GetVar(type,type);
        GetVar(name,name);
        GetVar(material,material);
        GetVar(units,units);
        GetVar(EOSlog,EOSlog);
        if( !strcmp(*argv,"-EOSlog") )
        {
            EOSlog = NULL;
            continue;
        }
        GetVar(nsteps,nsteps);
        // ref state
        GetVar(V0,V0);
        GetVar(e0,e0);
        GetVar(u0,u0);
        GetVar(P0,P0);
        GetVar(T0,T0);
        GetVar2(state, V0, e0);
        // init state
        if( !strcmp(*argv,"InitShock") )
        {
            InitLocus = WAVE::shock;
            continue;
        }
        if( !strcmp(*argv,"InitIsentrope") )
        {
            InitLocus = WAVE::isentrope;
            continue;
        }
        if( !strcmp(*argv,"u_p") )
            InitVar = VAR::Up;
        GetVar(u_p,var0);
        if( !strcmp(*argv,"u_s") )
            InitVar = VAR::Us;
        GetVar(u_s,var0);
        if( !strcmp(*argv,"Ps") )
            InitVar = VAR::P;
        GetVar(Ps,var0);
        if( !strcmp(*argv,"Vs") )
            InitVar = VAR::V;
        GetVar(Vs,var0);
        GetVar(lambda,lambda);
        GetVar(lambda2,lambda2);
        // wave directions
        GetVarValue(init_left,InitDir,LEFT);
        GetVarValue(init_right,InitDir,RIGHT);
        GetVarValue(CJleft,CJdir,LEFT);
        GetVarValue(CJright,CJdir,RIGHT);
        GetVarValue(left,dir,LEFT);
        GetVarValue(right,dir,RIGHT);
        GetVarValue(weak,weak,1);
        GetVarValue(release,release,1);
        GetVar(Pdet,Pdet);
        // locus: variable and range
        GetVar2(range,var1,var2);
        GetVar(var1,var1);
        GetVar(var2,var2);	
        if( !strcmp(*argv,"P") )
        {
            loop = VAR::P;
            continue;
        }
        if( !strcmp(*argv,"P2") )
            loop = VAR::P;
        GetVar(P2,var2);	
        if( !strcmp(*argv,"V") )
        {
            loop = VAR::V;
            continue;
        }
        if( !strcmp(*argv,"V2") )
            loop = VAR::V;
        GetVar(V2,var2);
        if( !strcmp(*argv,"u") )
        {
            loop = VAR::Up;
            continue;
        }
        if( !strcmp(*argv,"u2") )
            loop = VAR::Up;
        GetVar(u2,var2);	
        if( !strcmp(*argv,"us") )
        {
            loop = VAR::Us;
            continue;
        }
        if( !strcmp(*argv,"us2") )
            loop = VAR::Us;
        GetVar(us2,var2);
        // output format
        if( !strcmp(*argv, "P_form") )
        {
            char *ptr;
            P_form.width = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument P_form")
    		<< Exit;
            P_form.precision = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument P_form")
    		<< Exit;

            continue;
        }

        if( !strcmp(*argv, "V_form") )
        {
            char *ptr;
            V_form.width = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument V_form")
    		<< Exit;
            V_form.precision = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument V_form")
    		<< Exit;

            continue;
        }
        if( !strcmp(*argv, "e_form") )
        {
            char *ptr;
            e_form.width = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument V_form")
    		<< Exit;
            e_form.precision = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument V_form")
    		<< Exit;

            continue;
        }
        if( !strcmp(*argv, "u_form") )
        {
            char *ptr;
            u_form.width = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument V_form")
    		<< Exit;
            u_form.precision = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument V_form")
    		<< Exit;

            continue;
        }
        if( !strcmp(*argv, "us_form") )
        {
            char *ptr;
            us_form.width = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument V_form")
    		<< Exit;
            us_form.precision = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument V_form")
    		<< Exit;

            continue;
        }
        if( !strcmp(*argv, "c_form") )
        {
            char *ptr;
            c_form.width = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument V_form")
    		<< Exit;
            c_form.precision = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument V_form")
    		<< Exit;

            continue;
        }
        if( !strcmp(*argv, "?") || !strcmp(*argv,"help") )
            Help(0);
        ArgError;
    }
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
    // input check
    if( files==NULL )
        cerr << Error("must specify data file") << Exit;    
    if( lib )
    {
        setenv("EOSLIB_SHARED_LIBRARY_PATH",lib,1);
    }
    else if( !getenv("EOSLIB_SHARED_LIBRARY_PATH") )
    {
        cerr << Error("must specify lib or export EOSLIB_SHARED_LIBRARY_PATH")
             << Exit;  
    }
    lambda  = min(1.,max(0.,lambda));  
    lambda2 = max(0.,lambda2); // lambda2 > 1, no second reaction 
    if( material )
    {
        if( type || name )
    	    cerr << Error("Can not specify both material and name or type")
                 << Exit;
        if( TypeName(material,type,name) )
    	    cerr << Error("syntax error, material = ") << material
    	         << Exit;
    }
    else if( type == NULL || name == NULL )
    {
    	cerr << Error("Specify either material or name and type")
             << Exit;
    }
// Fetch eos
    DataBase db;
    if( db.Read(files) )
    	cerr << Error("Read failed" ) << Exit;
    if( EOSlog )
    {
        ofstream *EOS_log = new ofstream(EOSlog);
        //EOSbase::Error *Ehandler(*EOS_log); 
        //eos->ErrorHandler(&Ehandler);
        EOSbase::ErrorDefault = new EOSbase::Error(*EOS_log);
    }
    eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("material not found, ") << type << "::" << name << Exit;
    if( eos->ConvertUnits(units, db) )
    	cerr << Error("ConvertUnits failed") << Exit;
    HE = dynamic_cast<ExtEOS *>(eos);
    if( HE == NULL )
        cerr << Error("eos not ExtEOS") << Exit;
    //IDOF *zref =HE->Zref();
    //int i = zref->var_index("lambda");
    //if( i < 0 )
    //    cerr << Error("eos not HE") << Exit;
// ref state
    HydroState state0;
    state0.u = std::isnan(u0) ? 0 : u0;
    state0.V = std::isnan(V0) ? eos->V_ref : V0;
    state0.e = std::isnan(e0) ? eos->e_ref : e0;
    if( !std::isnan(P0) && !std::isnan(T0) )
	{
	    if( eos->PT(P0,T0,state0) )
	        cerr << Error("eos->PT failed\n") << Exit;
	}
// init state
   int status;
   WaveState InitState;
   if( std::isnan(var0) )
       status = eos->Evaluate(state0,InitState);
   else
   {
      Wave *InitWave = NULL;
      if( InitLocus==WAVE::shock )
          InitWave = eos->shock(state0);
      else
          InitWave = eos->isentrope(state0);
      if( InitWave == NULL )
          cerr << Error("InitWave is NULL") << Exit;
      if( InitVar == VAR::P )
          status = InitWave->P(var0,InitDir,InitState);
      else if( InitVar == VAR::V )
          status = InitWave->V(var0,InitDir,InitState);
      else if( InitVar == VAR::Up )
          status = InitWave->u(var0,InitDir,InitState);
      else if( InitVar == VAR::Us )
          status = InitWave->u_s(var0,InitDir,InitState);
      delete InitWave;
   }
   if( status )
       cerr << Error("InitState failed") << Exit;
    if( InitVar != VAR::none || std::isnan(P0) )
        P0 = eos->P(InitState);
    else
        InitState.P = P0;
// detonation wave
    Detonation *det;
    double *z = HE->z_f(InitState.V,InitState.e);
    if( lambda==1.0 && lambda2>=1.0 )
    {
        det = HE->detonation(InitState,P0);
        if( HE->Equilibrate(InitState.V,InitState.e,z) )
            cerr << Error("Equilibrate failed") << Exit;   
        HE = HE->Copy(z);       // fully burnt eos  
    }
    else
    {
        if( HE->set_z("lambda",lambda, z) )
            cerr << Error("set_z failed setting lambda") << Exit;
        if( lambda2 <= 1.0 )
        {
            if( HE->set_z("lambda2",lambda2, z) )
                cerr << Error("set_z failed setting lambda2") << Exit;
        }
        HE = HE->Copy(z);       // partially burnt eos  
        det =  HE->EOS::detonation(InitState,P0);
    }
    (void) HE->get_z(z, "lambda2",lambda2);     // used for print out
    if( det == NULL )
        cerr << Error("det is NULL") << Exit;
    WaveState CJstate;
    if( det->CJwave(CJdir,CJstate) )
        cerr << Error("CJwave failed") << Exit;
// Dstate and wave
    WaveState Dstate;
    Wave *wave;
    if( !std::isnan(Pdet) )
    {
        if( det->P(Pdet,dir,Dstate) )
            cerr << Error("det failed for Pwave") << Exit;
        wave=HE->isentrope(Dstate,z);
	    if( wave == NULL )
	        cerr << Error("isentrope(Pdet) failed\n") << Exit;
        delete det;
        det = NULL;
        release = 1;
    }
    else
    {
        Dstate = CJstate;
        wave = det;
    }
// set locus
    typedef int (Wave::*WaveLocus) (double, int, WaveState &);
    WaveLocus Locus = (WaveLocus)NULL;
    const char *lname;
    if( loop == VAR::none )
    {
        if( std::isnan(Pdet) )
            lname = "CJ detonation";
        else if( Pdet >= CJstate.P )
            lname = "strong detonation";
        else
            lname = "weak detonation";
    }
    else if( std::isnan(var2) )
        cerr << Error("var2 not set") << Exit;
    else
    {
        lname = "Detonation locus";
        if( loop == VAR::P )
        {
             Locus = &Wave::P;
             if( std::isnan(var1) )
                 var1 = Dstate.P;
             if( var2 < Dstate.P && std::isnan(Pdet) )
             {
                 if( weak )
                     lname = "Detonation (weak) locus";
                 else
                     release = 1;
             }
        }
        else if( loop == VAR::V )
        {
             Locus = &Wave::V;
             if( std::isnan(var1) )
                 var1 = Dstate.V;
             if( var2 > Dstate.V && std::isnan(Pdet) )
             {
                 if( weak )
                     lname = "Detonation (weak) locus";
                 else
                     release = 1;
             }
        }
        else if( loop == VAR::Up )
        {
             Locus = &Wave::u;
             if( std::isnan(var1) )
                 var1 = Dstate.u;
             if( dir*(var2-Dstate.u) < 0. && std::isnan(Pdet) )
             {
                 if( weak )
                     lname = "Detonation (weak) locus";
                 else
                     release = 1;
             }
        }
        else if( loop == VAR::Us )
        {
             Locus = &Wave::u_s;
             if( std::isnan(var1) )
                 var1 = Dstate.us;
             if( std::isnan(Pdet) )
             {
                 if( (var2-Dstate.us)*(var1-Dstate.us) < 0. )
                     cerr << Error("Range of us inconsistent\n") << Exit;
                 if( dir*(var2-Dstate.us) < 0. )
                     release = 1;
                 if( weak )
                 {
                     Locus = &Wave::w_u_s;
                     lname = "Detonation (weak) locus";
                 }
             }
        }
        if( release == 1 )
        {
            lname = "isentrope";
            if( std::isnan(Pdet) )
            {
	            delete wave;
	            if( (wave=HE->isentrope(CJstate)) == NULL )
	             cerr << Error("failed to initialize CJ isentrope\n")
                      << Exit;
            }
        }
    }
// print header
    cout << lname << "("
         << (dir == RIGHT ? "right" : "left") << ") for "
         << type << "::" << name << ",  ";
    cout << "lambda=" << setprecision(3) << fixed << lambda;
    if( lambda2 <= 1.0 )
      cout << "  lambda2=" << fixed << setprecision(3) << lambda2;
    cout << "\n";
    PrintState(InitState, "init state");
    if( loop == VAR::none  && (std::isnan(Pdet) || Pdet >= CJstate.P) )
    {
        WaveState VN;
        Wave *shock = eos->shock(InitState);
        if( shock == NULL )
            cerr << Error("Shock is NULL") << Exit;
        if( shock->u_s(CJstate.us, dir,VN) )
            cerr << Error("VN state  failed") << Exit;        
        PrintState(VN, "VN state");
        delete shock;
    }
    //
    PrintLine();
    WaveStateLabel(cout) << " "
                         << Center(c_form, "c")  << " "
                         << Center(FD_form,"FD") << " "
                         << Center(T_form, "T")  << "\n";
    const Units *u = eos->UseUnits();
    if( u )
    {
        WaveStateLabel(cout, *u) << " "
            << Center(u_form,u->Unit("velocity")) << " "
            << Center(FD_form," ") << " "
            << Center(T_form,u->Unit("temperature")) << "\n";
    }
    PrintLine();
    // reset eos for partial reaction (needed for T & c on locus)
    deleteEOS(eos);
    eos = HE;
    if( loop == VAR::none )
    {
        PrintState(Dstate,"Det state");
        return 0;
    }
//  locus
    double var;
    double dvar = (var2-var1)/nsteps;
    int count;
    WaveState wstate;
    for(count=nsteps, var=var1; count--; var += dvar)
    {
        if( (wave->*Locus)(var,dir,wstate) )
            cerr << Error("wave->locus failed") << Exit;
        PrintState(wstate);
    }
    if( (wave->*Locus)(var2,dir,wstate) )
        cerr << Error("wave->locus failed") << Exit;
    PrintState(wstate);
    delete wave;
    deleteEOS(eos);
    return 0;
}
