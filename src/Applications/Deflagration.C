#include <Arg.h>
#include <ExtEOS.h>

#define NaN EOS::NaN

EOS *eos    = NULL;
Format FD_form;


void PrintState(WaveState &state)
{
    double c  = eos->c(state.V,state.e);
    double cT = sqrt(state.V*eos->KT(state.V,state.e));
    double FD = eos->FD(state.V,state.e);
    double T  = eos->T(state.V,state.e);

    cout << state   << " "
         << c_form  << c  << " "
         << FD_form << FD << " "
         << T_form  << T  << " "
         << c_form  << cT << "\n";
}

void PrintUnits()
{
    const Units *u = eos->UseUnits();
    if( u )
    {
        WaveStateLabel(cout, *u) << " "
            << Center(u_form,u->Unit("velocity")) << " "
            << Center(FD_form," ") << " "
            << Center(T_form,u->Unit("temperature")) << " "
            << Center(u_form,u->Unit("velocity")) << "\n";
    }
}
void PrintLabels()
{
    WaveStateLabel(cout) << " "
                         << Center(c_form, "c")  << " "
                         << Center(FD_form,"FD") << " "
                         << Center(T_form, "T")  << " "
                         << Center(c_form, "cT") << "\n";
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
         << setw(T_form.width)  << "" << "-"
         << setw(c_form.width)  << "" << "\n";
    cout.fill(' ');
}

const char *help[] = {    // list of commands printed out by help option
    "name        name    # material name",
    "type        name    # EOS type",
    "material    name    # type::name",
    "file[s]     file    # : separated list of data files [EOS.data]",
    "units       name    # units from data base [hydro.std]",
    "",
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
    "u_p    value   # shock particle velocity",
    "u_s    value   # shock velocity",
    "Ps     value   # shock pressure",
    "Vs     value   # shock specific volume",
    "",
    "Deflagration locus:",
    "lambda value   # partially burned locus (reactive ExtEOS only)",
    "left           # left facing wave",
    "right          # right facing wave (default)",
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

    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/EOS.data";
    const char * files = file_.c_str(); 
    //const char *files    = "EOS.data";    	
    const char *type     = NULL;
    const char *name     = NULL;
    const char *material = NULL;
    const char *units    = "hydro::std";

    int nsteps = 10;

    double V0 = NaN;
    double e0 = NaN;
    double u0 = NaN;
    double P0 = NaN;
    double T0 = NaN;

    WAVE::type InitLocus = WAVE::shock;
    int  InitDir         = RIGHT;	
    VAR::type  InitVar   = VAR::none;
    double var0          = NaN;    // value on init state locus
    double lambda        = 1.;     // partially burned locus

    int dir        = RIGHT;
    VAR::type loop = VAR::none;
    double var1    = NaN;
    double var2    = NaN;
// process command line arguments
    if( argv[1] == NULL )
        Help(-1);
    while(*++argv)
    {
        GetVar(file,files);
        GetVar(files,files);
        GetVar(type,type);
        GetVar(name,name);
        GetVar(material,material);
        GetVar(units,units);

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
        // wave directions
        GetVarValue(init_left,InitDir,LEFT);
        GetVarValue(init_right,InitDir,RIGHT);
        GetVarValue(left,dir,LEFT);
        GetVarValue(right,dir,RIGHT);
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
        if( !strcmp(*argv, "?") || !strcmp(*argv,"help") )
            Help(0);
        ArgError;
    }
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);		
    lambda =min(1.,max(0.,lambda));  
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
    eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("material not found, ") << type << "::" << name << Exit;
    if( eos->ConvertUnits(units, db) )
    	cerr << Error("ConvertUnits failed") << Exit;
    ExtEOS *HE = dynamic_cast<ExtEOS *>(eos);
    if( HE == NULL )
        cerr << Error("eos not ExtEOS") << Exit;
    IDOF *zref =HE->Zref();
    int i = zref->var_index("lambda");
    if( i < 0 )
        cerr << Error("eos not HE") << Exit;
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
// deflagration wave and CJ state
    double *z = HE->z_f(InitState.V,InitState.e);
    z[i] = lambda;
    HE = HE->Copy(z);       // partially burnt eos  
    P0 = eos->P(InitState);
    Deflagration *def =  HE->EOS::deflagration(InitState,P0);
    if( def == NULL )
        cerr << Error("det is NULL") << Exit;   
    WaveState CJ;
    if( def->CJwave(dir,CJ) )
        cerr << Error("CJwave failed") << Exit;
    Wave *wave = def;
// set locus
    const char *lname = NULL;
    typedef int (Wave::*WaveLocus) (double, int, WaveState &);
    WaveLocus Locus = (WaveLocus)NULL;
    if( loop == VAR::none )
        lname = "CJ Deflagration";
    else if( std::isnan(var2) )
        cerr << Error("var2 not set") << Exit;
    else
    {
        lname = "Deflagration locus";
        int range = 0;
        if( loop == VAR::P )
        {
             Locus = &Wave::P;
             if( std::isnan(var1) )
                 var1 = CJ.P;
             if( var2 < CJ.P )
                 range = 1;
        }
        else if( loop == VAR::V )
        {
             Locus = &Wave::V;
             if( std::isnan(var1) )
                 var1 = CJ.V;
             if( var2 > CJ.V )
                 range = 1;
        }
        else if( loop == VAR::Up )
        {
             Locus = &Wave::u;
             if( std::isnan(var1) )
                 var1 = CJ.u;
             if( dir*(var2-CJ.u) < 0. )
                 range = 1;
        }
        else if( loop == VAR::Us )
        {
             Locus = &Wave::u_s;
             if( std::isnan(var1) )
                 var1 = CJ.us;
             if( dir*(var2-CJ.us) > 0. )
                 range = 1;
        }
        if( range == 1 )
            cerr << Error("var2 not on weak branch") << Exit;
    }
// print header
    cout << lname << "("
         << (dir == RIGHT ? "right" : "left") << ") for "
         << type << "::" << name << "\n";
    PrintState(InitState);
    PrintLine();
    PrintLabels();
    PrintUnits();
    PrintLine();
// reset eos for partial reaction (needed for T & c on locus)
    deleteEOS(eos);
    eos = HE;
    if( loop == VAR::none )
    {
        PrintState(CJ);
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
