#include <Arg.h>
#include <LocalMath.h>
#include <EOS.h>
#include <ShockPolar_gen.h>
#include <PrandtlMeyer_ODE.h>

const char *help[] = {    // list of commands printed out by help option
    "locus of shock polar / Prandtl-Meyer fan",
    "material:",
    "  name        name    # material name",
    "  type        name    # EOS type",
    "  material    name    # type::name",
    "  file[s]     file    # : separated list of data files [EOS.data]",
    "  units       name    # default units from data base [hydro::std]",
    "initial polar state:",
    "  V0          value   # specific volume of reference state [V_ref]",
    "  e0          value   # specific energy of reference state [e_ref]",
    "  state       V0 e0   # reference state",
    "  q0          value   # incoming particle speed",
    "  theta0      value   # angle between incoming particle path and x-axis [0]",
    "  M0          value   # q0 = M0*c0",
    "  Ps          value   # incident polar with Pmax=Ps",
    "  P0          value   # q0 from pressure P0 on incident polar",
    "  left0               # left  facing incident polar",
    "  right0              # right facing incident polar",
    "loop P from P1 to P2:",
    "  P1          value   # [P0   on shock polar]",
    "  P2          value   # [Pmax on shock polar]",
    "  nsteps      value   # number of points on polar",
    "  left                # left  facing wave",
    "  right               # right facing wave",
    "  states              # print distinguished states on polar",
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

namespace PRINT
{
    enum type { loop, states };
}
//
//
int main(int, char **argv)
{
    ProgName(*argv);
    EOS::Init();
    InitFormat();
    // material
    const char *files    = "EOS.data";    
    const char *type     = NULL;
    const char *name     = NULL;
    const char *material = NULL;
    const char *units    = "hydro::std";
    // initial polar state
    double     V0 = NaN;    // default: eos->V_ref
    double     e0 = NaN;    // default: eos->e_ref
    double     q0 = NaN;
    double     M0 = NaN;    // q0 = M0*c0
    double theta0 = 0.0;
    double Ps = NaN;        // shock pressure for q0
    double P0 = NaN;        // point on incident polar
    int dir0 = RIGHT;       // direction for incident polar
    // loop P= P1->P2 with nsteps
    int dir1 = RIGHT;       // polar direction
    double P1 = NaN;        // default: Polar.P0
    double P2 = NaN;        // default: Polar.Pmax
    int nsteps = 10;
    int output = PRINT::loop;   // flag for type of output
// process command line arguments
    if( argv[1] == NULL )
        Help(-1);
    while(*++argv)
    {
        // material
        GetVar(file,files);
        GetVar(files,files);
        GetVar(type,type);
        GetVar(name,name);
        GetVar(material,material);
        GetVar(units,units);
        // initial state        
        GetVar(V0,V0);
        GetVar(e0,e0);
        GetVar2(state, V0, e0);
        GetVar(q0,q0);
        GetVar(M0,M0);
        GetVar(theta0,theta0);
        GetVar(Ps,Ps);
        GetVar(P0,P0);
        GetVarValue(left0,dir0,LEFT);
        GetVarValue(right0,dir0,RIGHT);
        // loop
        GetVar(P1,P1);
        GetVar(P2,P2);        
        GetVarValue(left,dir1,LEFT);
        GetVarValue(right,dir1,RIGHT);
        GetVar(nsteps,nsteps);        
        GetVarValue(states,output,PRINT::states);
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
// fetch eos
    DataBase db;
    if( db.Read(files) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("material not found, ") << type << "::" << name << Exit;    
    if( eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    // polar state
    PolarState pstate0;
    pstate0.V = std::isnan(V0) ? eos->V_ref : V0;
    pstate0.e = std::isnan(e0) ? eos->e_ref : e0;
    pstate0.q     = q0;
    pstate0.theta = theta0;
    if( !std::isnan(Ps) )
    {
        HydroState state0(pstate0.V,pstate0.e);        
        Hugoniot *hug = eos->shock(state0);
        if( hug==NULL )
            cerr << Error("eos->shock failed") << Exit;
        WaveState ws;
        if( hug->P(Ps,RIGHT,ws) )
            cerr << Error("hug->P failed") << Exit;
        pstate0.q = ws.us;
        delete hug;
    }
    else if( !std::isnan(M0) )
    {
        if( M0 <= 1 )
            cerr << Error("M0 <= 1") << Exit;
        double c0 =eos->c(pstate0.V,pstate0.e);
        pstate0.q = M0*c0;
    }
    else if( std::isnan(pstate0.q) )
        cerr << Error("polar state not specified") << Exit;
    //
    ShockPolar_gen Polar(*eos);
    Polar.P_tol = 0.0;
    if( Polar.Initialize(pstate0) )
        cerr << Error("Polar.Initialize(pstate0) failed") << Exit;
    PolarWaveState pwave0;
    if( !std::isnan(P0) )
    {
        if( Polar.P(P0,dir0,pwave0) )
            cerr << Error("Polar.P(P0) failed") << Exit;
        if( Polar.Initialize(pwave0) )
            cerr << Error("Polar.Initialize(pwave0) failed") << Exit;
    }
    PrandtlMeyer_ODE PM(*eos);
    if( std::isnan(P0) )
    {
        if( PM.Initialize(pstate0) )
            cerr << Error("PM.Initialize(state0) failed") << Exit;
    }
    else
    {
        if( PM.Initialize(pwave0) )
            cerr << Error("PM.Initialize(pwave0) failed") << Exit;
    }    
    if( output == PRINT::states )
    {
        PolarWaveStateLabel(cout<<setw(12)<<"")
            << " " << Center(M_form, "M") << "\n";
        PolarWaveStateLabel(cout<<setw(12)<<"",*eos->UseUnits()) << "\n";
        PolarWaveState Pwave;
        double M;
        Pwave.V     = Polar.state0.V;
        Pwave.e     = Polar.state0.e;
        Pwave.q     = Polar.state0.q;
        Pwave.theta = Polar.state0.theta;
        Pwave.P     = Polar.P0;
        Pwave.beta  = Pwave.theta+dir1*Polar.beta0_min;
        M = Pwave.q/eos->c(Pwave.V,Pwave.e);
        cout << setw(12) << "        P0: " << Pwave
                                           << " " << M_form << M << "\n";
        M = Polar.sonic.q/eos->c(Polar.sonic.V,Polar.sonic.e);
        cout << setw(12) << "     sonic: " << Polar.sonic         
                                           << " " << M_form << M << "\n";
        M = Polar.dtheta_max.q/eos->c(Polar.dtheta_max.V,Polar.dtheta_max.e);
        cout << setw(12) << "dtheta_max: " << Polar.dtheta_max
                                           << " " << M_form << M << "\n";
        Pwave.V     = Polar.Pmax.V;
        Pwave.e     = Polar.Pmax.e;
        Pwave.q     = Polar.Pmax.us-Polar.Pmax.u;
        Pwave.theta = Polar.state0.theta;
        Pwave.P     = Polar.Pmax.P;
        Pwave.beta  = Pwave.theta + dir1*0.5*PI;
        M = Pwave.q/eos->c(Pwave.V,Pwave.e);
        cout << setw(12) << "     P_max: " << Pwave
                                           << " " << M_form << M << "\n";
        return 0;
    }
    if( std::isnan(P1) )
        P1 = Polar.P0;
    if( std::isnan(P2) )
        P2 = Polar.Pmax.P;
    //
    PolarWaveStateLabel(cout) << " " << Center(M_form, "M") << "\n";  
    PolarWaveStateLabel(cout,*eos->UseUnits()) << "\n";  
    int i;
    for( i=0; i<=nsteps; i++ )
    {
        double p = P1 + double(i)/double(nsteps)*(P2-P1);
        PolarWaveState pwave;
        if( p >= Polar.P0 )
        {
            p = max(p,(1.+1e-3)*Polar.P0);
            if( Polar.P(p,dir1,pwave) )
                cerr << Error("Polar.P failed") << Exit;
        }
        else
        {
            p = min(p,(1.-1e-3)*Polar.P0);
            if( PM.P(p,dir1,pwave) )
                cerr << Error("PM.P failed") << Exit;
        }
        double c = eos->c(pwave.V,pwave.e);
        cout << pwave
             << " " << M_form << pwave.q/c
             << "\n";
    }
    deleteEOS(eos);        
    return 0;
}
