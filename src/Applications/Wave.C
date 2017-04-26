#include <Arg.h>
#include <ExtEOS.h>
#include <Porous.h>

#define NaN EOS::NaN

EOS *eos    = NULL;
EqPorous *phi_eq = NULL;
int print_S = 0 ;   // flag to print entropy

Format FD_form;
Format phi_form;
Format S_form;

void PrintLabel()
{            
    WaveStateLabel(cout) << " "
                         << Center(c_form, "c")  << " "
                         << Center(FD_form,"FD") << " "
                         << Center(T_form, "T");
    if( print_S )
        cout << " " << Center(S_form,"S");
    if( phi_eq )
        cout << " " << Center(phi_form,"phi") << " " << Center(e_form,  "B");
    cout << "\n";
    const Units *u = eos->UseUnits();
    if( u )
    {
        WaveStateLabel(cout, *u) << " "
                                 << Center(u_form,u->Unit("velocity")) << " "
                                 << Center(FD_form," ") << " "
                                 << Center(T_form,u->Unit("temperature"));
        if( print_S )
            cout << " " << Center(S_form,u->Unit("S"));
        if( phi_eq )
            cout << " " << Center(phi_form," ")
                 << " " << Center(e_form,u->Unit("specific_energy"));
        cout << "\n";
    }
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
         << setw(T_form.width)  << "" ;
    if( print_S )
        cout << setw(S_form.width)  << "" << "-";
    if( phi_eq )
    {
        cout << setw(phi_form.width)  << "" << "-"
             << setw(e_form.width)    << "" << "-";

    }
    cout << "\n";
    cout.fill(' ');
}

void PrintState(WaveState &state)
{            
    double c = sqrt(eos->c2(state.V,state.e));
    double FD = eos->FD(state.V,state.e);
    double T = eos->T(state.V,state.e);
        
    cout << state << " "
         << c_form  << c << " "
         << FD_form << FD << " "
         << T_form  << T;
    if( print_S )
    {
        double S = eos->S(state.V,state.e);
        cout << " " << S_form << S;
    }
    if( phi_eq )
    {
        double phi = phi_eq->phi(state.P*state.V);
        cout << " " << phi_form << phi
             << " " << e_form   << phi_eq->energy(phi);
    }
    cout << endl;
}

const char *help[] = {    // list of commands printed out by help option
    "name        name    # material name",
    "type        name    # EOS type",
    "material    name    # type::name",
    "units       name    # units for Wave",
    "file[s]     file    # colon separated list of data files",
    "lib         name    # directory for EOSlib shared libraries",
    "                    # default environ variable EOSLIB_SHARED_LIBRARY_PATH",
    "",
    "Type of locus, default prints ref state only",
    "isentrope      # compute isentrope",
    "shock          # compute shock locus",
    "detonation     # compute detonation locus",
    "left           # left facing wave",
    "right          # right facing wave",
    "u_escape       # expansion to escape velocity (cavitation) state",
    "valid_u        # limits u2 to escape velocity on isentrope",
    "weak           # weak detonation branch for us",
    "",
    "Reference state, default V_ref, e_ref, u=0 from database",
    "V0     value   # specific volume",
    "e0     value   # specific energy",
    "state  V0 e0   # reference state",
    "P0     value   # pressure of reference state",
    "T0     value   # Temperature of reference state",
    "PT     P0 T0   # reference state",
    "u0     value   # velocity of reference state",
    "",
    "Initial state for locus, default is reference state",
    "InitShock      # Initial state on shock locus based on ref state",
    "InitIsentrope  # Initial state on isentrope from ref state",
    "init_left      # left  facing wave for initial state",
    "init_right     # right facing wave for initial state",
    "u_p   value    # shock particle velocity",
    "u_s   value    # shock velocity",
    "Ps    value    # shock pressure",
    "Vs    value    # shock specific volume",
    "",
    "Wave curve by variable",
    "P              # wave curve by P",
    "u              # wave curve by u",
    "us             # wave curve by us",
    "V              # wave curve by V",
    "",
    "range   var1 var2    # range of wave curve",
    "var1    value,       # default is shock state",
    "var2    value",
    "",
    "P2    value    # P  & var2=value",
    "u2    value    # u  & var2=value",
    "us2   value    # us & var2=value",
    "V2    value    # V  & var2=value",
    "",
    "nsteps    value    # number of points on wave curve",
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
    enum type { none, P, Up, Us, V, CJ };
}
namespace WAVE
{
    enum type { state, shock, isentrope, detonation };
}
namespace ESCAPE
{
    enum type { off, limit, valid };
}
//
//
int main(int, char **argv)
{
    ProgName(*argv);
    EOS::Init();
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
    VAR::type  InitVar   = VAR::none;
    double var0          = NaN;         // value of var on InitLocus
    
    ESCAPE::type u_esc   = ESCAPE::off; // flag to compute escape velocity        
    WAVE::type locus     = WAVE::state;
    int dir              = RIGHT;
    int weak             = 0;           // weak detonation
    VAR::type loop       = VAR::none;
    double var1          = NaN;
    double var2          = NaN;
        
// process command line arguments
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
        GetVar2(state, V0, e0);
        GetVar(P0,P0);
        GetVar(T0,T0);
        GetVar2(PT, P0, T0);
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
        // wave directions       
        GetVarValue(init_left,InitDir,LEFT);
        GetVarValue(InitLeft,InitDir,LEFT);
        GetVarValue(init_right,InitDir,RIGHT);
        GetVarValue(InitRight,InitDir,RIGHT);
        GetVarValue(left,dir,LEFT);
        GetVarValue(right,dir,RIGHT);
        // locus
        if( !strcmp(*argv,"u_escape") )
        {
            u_esc = ESCAPE::limit;
            continue;
        }
        if( !strcmp(*argv,"valid_u") )
        {
            u_esc = ESCAPE::valid;
            continue;
        }        
        if( !strcmp(*argv,"isentrope") || !strcmp(*argv,"Isentrope") )
        {
            locus = WAVE::isentrope;
            continue;
        }
        if( !strcmp(*argv,"shock") || !strcmp(*argv,"Shock") )
        {
            locus = WAVE::shock;
            continue;
        }
        if( !strcmp(*argv,"detonation") || !strcmp(*argv,"Detonation") )
        {
            InitVar = VAR::CJ;
            continue;
        }
        if( !strcmp(*argv,"weak") || !strcmp(*argv,"Weak") )
        {
            InitVar = VAR::CJ;
            locus = WAVE::detonation;
            continue;
        }
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
        // format
        GetVarValue(printS,print_S,1);
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
               cerr << Error("following command line argument e_form")
                    << Exit;
            e_form.precision = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument e_form")
                    << Exit;          
            continue;
        }
        if( !strcmp(*argv, "u_form") )
        {
            char *ptr;
            u_form.width = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument u_form")
                    << Exit;
            u_form.precision = strtol(*++argv,&ptr,10);
            if( *ptr != '\0' )
               cerr << Error("following command line argument u_form")
                    << Exit;          
            continue;
        }
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
    if( u_esc != ESCAPE::off )
        locus = WAVE::isentrope;
    if( loop!=VAR::none && std::isnan(var2) )
        cerr << Error("var2 not set") << Exit;    
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
        cerr << Error("must specify either material or name and type")
             << Exit;
    }
    if( u_esc != ESCAPE::off )
        locus = WAVE::isentrope;
    if( loop!=VAR::none && std::isnan(var2) )
        cerr << Error("var2 not set") << Exit;    
//
// fetch eos
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
    phi_eq=dynamic_cast<EqPorous *>(eos);
// ref state
    HydroState state0;        
    state0.u = std::isnan(u0) ? 0 : u0;
    state0.V = std::isnan(V0) ? eos->V_ref : V0;
    state0.e = std::isnan(e0) ? eos->e_ref : e0;
    if( InitVar == VAR::CJ )
    {
        u_esc = ESCAPE::off;
        if( std::isnan(P0) )
            P0 = 0.;
    }
    else if( !std::isnan(P0) || !std::isnan(T0) )
    {
      if( std::isnan(P0) && std::isnan(P0=eos->P(state0)) )
            cerr << Error("eos->P returned NaN\n") << Exit;
        if( std::isnan(T0) )
            T0 = eos->T(state0);
        if( eos->PT(P0,T0,state0) )
            cerr << Error("eos->PT failed\n") << Exit;
    }
    else
        if( std::isnan(P0=eos->P(state0)) )
            cerr << Error("eos->P returned NaN\n") << Exit;
// init state
    WaveState InitState;
    Wave *wave = NULL;
    if( InitVar == VAR::CJ )
    {
        Detonation *det = eos->detonation(state0,P0);
        if( det == NULL )
            cerr << Error("detonation = NULL\n") << Exit;
        if( det->CJwave(InitDir, InitState) )
            cerr << Error("CJstate failed\n") << Exit;
        wave = det;
    }
    else if( InitVar == VAR::none )
    {
        if( eos->Evaluate(state0,InitState) )
            cerr << Error("eos->Evaluate failed") << Exit;
        if( InitDir == LEFT )
            InitState.us *= -1.;
    }
    else
    {
        if( std::isnan(var0) )
            cerr << Error("must specify variable for InitLocus") << Exit;                       
        Wave *InitWave = NULL;
        if( InitLocus==WAVE::shock )
            InitWave = eos->shock(state0);
        else
            InitWave = eos->isentrope(state0);
        if( InitWave == NULL )
            cerr << Error("Wave *InitWave = NULL\n") << Exit;
        int status = 0;
        if( InitVar == VAR::P )
            status = InitWave->P(var0,InitDir,InitState);
        else if( InitVar == VAR::V )
            status = InitWave->V(var0,InitDir,InitState);
        else if( InitVar == VAR::Up )
            status = InitWave->u(var0,InitDir,InitState);
        else if( InitVar == VAR::Us )
            status = InitWave->u_s(var0,InitDir,InitState);
        delete InitWave;
        if( status )
            cerr << Error("InitWave failed\n"
                    "check InitValue compatible with InitState, InitLocus & InitDir\n"
                    "    shock in compression\n"
                    "    isentrope above P vacuum\n")
                 << Exit;
        /***
         *      ToDo: set default similar to locus
         *            ie use shock in compression or else isentrope
        ***/
    }
// select Locus (P, V, u or u_s) and branch (isentrope or shock)    
    typedef int (Wave::*WaveLocus) (double, int, WaveState &);
    int (Wave::*Locus)(double,int,WaveState&) = (WaveLocus)NULL;
    
    WAVE::type branch = WAVE::state;
    WaveState escape;
    const char*  lname = NULL;
    if( u_esc != ESCAPE::off )
    {
        Isentrope *I = eos->isentrope(InitState);
        if( I == NULL )
            cerr << Error("failed to initialize escape isentrope\n") << Exit;
        if( I->u_escape(dir,escape) )
            cerr << Error("u_escape failed\n") << Exit;
        wave = I; 
        lname = "Escape Isentrope";
        if( u_esc == ESCAPE::valid )
        {
            if( loop == VAR::P )
            {
                Locus = &Wave::P;
                var1 = InitState.P;
                var2 = escape.P;
            }
            else if( loop == VAR::V )
            {
                Locus = &Wave::V;
                var1 = InitState.V;
                var2 = escape.V;
            }
            else if( loop == VAR::Us )
            {
                Locus = &Wave::u_s;
                var1 = InitState.us;
                var2 = escape.us;
            }
            else
            {
                Locus = &Wave::u;
                var1 = InitState.u;
                var2 = escape.u;
            }         
        }     
    }
    else if( loop != VAR::none )
    {
        if( std::isnan(var2) )
            cerr << Error("var2 not set") << Exit;    
        if( loop == VAR::P )
        {
            Locus = &Wave::P;
            if( std::isnan(var1) )
                var1 = InitState.P;        
            branch = (var2 < InitState.P) ? WAVE::isentrope : WAVE::shock;
        }
        else if( loop == VAR::V )
        {
            Locus = &Wave::V;
            if( std::isnan(var1) )
                var1 = InitState.V;        
            branch = (var2 > InitState.V) ? WAVE::isentrope : WAVE::shock;
        }
        else if( loop == VAR::Up )
        {
            Locus = &Wave::u;
            if( std::isnan(var1) )
                var1 = InitState.u;        
            branch = (dir*(InitState.u-var2) > 0.) ? WAVE::isentrope : WAVE::shock;
        }
        else if( loop == VAR::Us )
        {
            Locus = &Wave::u_s;
            double c = sqrt(eos->c2(InitState.V, InitState.e));
            if( std::isnan(var1) )
                var1 = InitState.us;        
            branch = (dir*(var2-InitState.us) > 0.) ? WAVE::shock : WAVE::isentrope;
        }

        if( locus == WAVE::state )
            locus = branch;

    // set wave (shock, isentrope or detonation)
        if( InitVar==VAR::CJ )
        {
            if( branch == WAVE::state )
                lname = "CJ Detonation";
            else if( locus==WAVE::detonation )
            {
                lname = "Detonation";
            }
            else if( branch==WAVE::isentrope || locus==WAVE::isentrope )
            {
                delete wave;
                if( (wave = eos->isentrope(InitState)) == NULL )
                    cerr << Error("failed to initialize CJ isentrope\n") << Exit;
                lname = "CJ isentrope";
            }
            else
                lname = "Detonation";
            locus = WAVE::detonation;
        }
        else if( locus == WAVE::isentrope )
        {
            if( (wave=eos->isentrope(InitState)) == NULL )
                    cerr << Error("failed to initialize isentrope\n") << Exit;
            lname = "Isentrope";
        }
        else if( locus == WAVE::shock )
        {
            if( (wave = eos->shock(InitState)) == NULL )
                cerr << Error("failed to initialize Hugoniot\n") << Exit;
            lname = "Hugoniot";
        }
    }
    else if( InitVar==VAR::CJ )
        locus = WAVE::detonation;


    if( lname )
        cout << lname << " (" << (dir == RIGHT ? "right" : "left") << ") for ";
// print header
    cout << type << "::" << name << "\n";         
    WaveState wstate;    
    if( InitVar==VAR::CJ )
    {
        eos->Evaluate(state0, wstate);
        wstate.P = P0;
        PrintState(wstate);
        PrintLine();    
    }
    else if( loop!=VAR::none )
    {
        eos->Evaluate(InitState, wstate);
        if( locus == WAVE::detonation )
            wstate.P = P0;
        PrintState(wstate);
        PrintLine();    
    }
    else if( InitVar!=VAR::none )
    {
        eos->Evaluate(state0, wstate);
        PrintState(wstate);
        PrintLine();    
    }
    PrintLabel();
    PrintLine();
//
    if( locus==WAVE::detonation )
    {
        ExtEOS *HE = dynamic_cast<ExtEOS *>(eos);
        if( HE )
        {
            IDOF *zref =HE->Zref();
            int i = zref->var_index("lambda");
            if( i>=0 )
            {
                HE->Equilibrium();
                HE->c2_tol = 1e-6;
            }
        }
    }
//
    if( wave == NULL || (locus==WAVE::detonation && loop==VAR::none) )
    {
        PrintState(InitState);
    }
    else if( u_esc == ESCAPE::limit )
    {
        PrintState(escape);        
    }
    else
    {    
        // print wave locus
        if( Locus == (WaveLocus)NULL )
           cerr << Error("wave->*Locus not set, input error\n")
                << "\tneed arguments: P range var 1 var 2 or P2 var2, etc.\n"
                << Exit;        
        double var;
        double dvar = (var2-var1)/nsteps;
        int count;    
        int status;
        for(count=nsteps, var=var1; count-- >0; var += dvar)
        {
            status = (wave->*Locus)(var,dir,wstate);
            if( status )
                cerr << Error("wave->locus failed")
                     << " with status " << status << Exit;
            PrintState(wstate);
        }
        // avoid round off error on var = var2
        if( u_esc == ESCAPE::valid )
            wstate = escape;
        else if( (status = (wave->*Locus)(var2,dir,wstate)) )
            cerr << Error("wave->locus failed") 
                 << " with status " << status << Exit;
        PrintState(wstate);
    }
// test reference counting
    delete wave;
    deleteEOS(eos);
    return 0;
}
