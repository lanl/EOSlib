#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include <HEburn.h>
#include <Arrhenius.h>
#include <EOS_VT.h>
#include <PTequilibrium.h>
#include <PTequilibrium_VT.h>
//
ostream& PrintState(EOS &eos, WaveState &state)
{
    double c  = sqrt(eos.c2(state.V,state.e));
    double T  = eos.T(state.V,state.e);
    double cT = sqrt(state.V*eos.KT(state.V,state.e));
    cout << state  << " "
         << c_form << c  << " "
         << T_form << T  << " "
         << c_form << cT ;
    return cout;
}
void PrintUnits(EOS &eos)
{
    const Units *u = eos.UseUnits();
    if( u )
    {
        WaveStateLabel(cout, *u) << " "
            << Center(u_form,u->Unit("velocity"))    << " "
            << Center(T_form,u->Unit("temperature")) << " "
            << Center(u_form,u->Unit("velocity"))    << "\n";
    }
}
void PrintLabels()
{
    WaveStateLabel(cout) << " "
                         << Center(c_form, "c")  << " "
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
         << setw(T_form.width)  << "" << "-"
         << setw(c_form.width)  << "" << "\n";
    cout.fill(' ');
}
//
//
class DefProfile : public ODE
{
// Deflagration profile determined by ODEs
//
//      (Vh,Th) = intersection of partly burned Hugoniot & Rayleigh line
//                that is, right hand side vanishes for (d/dzeta) (T & V)
// (d/lambda) Vh
// (d/lambda) Th
// where
//   m = rho*(D-u) is mass flux
public:
    Arrhenius *rate;
    //
    PTequilibrium_VT *HE;   // rate->HE->eosVT()
    EOS_VT *reactants;      // HE->eos1
    EOS_VT *products;       // HE->eos2
    //
    double V0, T0, lambda0;
    double D,u0;        // Deflagration wave speed and ahead velocity
    double m, m2;       // mass flux, m = (D-u0)/V0
    double P0,e0;
    double y[3],yp[3];
    DefProfile(Arrhenius *Arate);
    ~DefProfile();
    void set_lambda(double lambda);
    double P(double lambda, double V, double T);
    double e(double lambda, double V, double T);
    double  Rate(double lambda, double V, double T);
    int Init(double lambda, double V, double T, double wave_speed, double u);
    int F(double *yp_z, const double *y_z, double lambda); // ODE::F 
    int Fext(double *yp_z, const double *y_z, double lambda, double lambda_CJ);
    int refine(double lambda, double &V, double &T);
    double MaxNorm(const double *y0, const double *y1,
                   const double*,    const double*);
};
DefProfile::DefProfile(Arrhenius *Arate) : ODE(2,512), rate(Arate)
{
    if( rate == NULL )
        cerr << Error("bad argument to DefProfile") << Exit;
    if( rate->HE == NULL )
        cerr << Error("rate->HE") << Exit;
    HE = dynamic_cast<PTequilibrium_VT *>(rate->HE->eosVT());
    if( HE == NULL )
        cerr << Error("HE_VT is NULL") << Exit;
    reactants = HE->eos1;
    products  = HE->eos2;
}
DefProfile::~DefProfile()
{
    EOS_VT *eosVT = HE;
    deleteEOS_VT(eosVT);
}
void DefProfile::set_lambda(double lambda)
{
        HE->lambda1 = 1.-lambda;
        HE->lambda2 = lambda;
}
double DefProfile::P(double lambda, double V, double T)
{
    set_lambda(lambda);
    return HE->P(V,T);
}
double DefProfile::e(double lambda, double V, double T)
{
    set_lambda(lambda);
    return HE->e(V,T);
}
double DefProfile::Rate(double lambda, double V, double T)
{
    return rate->Rate(lambda,T);
}
//
int DefProfile::Init(double lambda,  double V, double T,
                     double w_speed, double u)
{
    lambda0 = lambda;
    y[0] = V0 = V;
    y[1] = T0 = T;
    u0 = u;
    D = w_speed;
    m = (D-u0)/V0;
    m2 = m*m;
    //
    set_lambda(lambda0);
    P0 = HE->P(V0,T0);
    e0 = HE->e(V0,T0);
    //
    double dlambda = 1.e-4;
    // ahead state (V0,T0) at lambda0
    // integrate to behind state at zeta < 0
    int status = Initialize(lambda0, y, dlambda); 
    return status;
}
int DefProfile::F(double *yp_z, const double *y_z, double lambda)
{
    return Fext(yp_z,y_z,lambda, 0.999);
}
int DefProfile::Fext(double *yp_z, const double *y_z, double lambda,
                     double lambda_CJ)
{
    double Vh = y_z[0];
    double Th = y_z[1];
    // temporary fix to guarantee HE->V1 and HE->V2 are calculated
    if( lambda <= 0.0 )
        lambda = 1e-10;
    else if( lambda >= 1.-1.e-10 )
    {
        yp_z[0] = 0.0;
        yp_z[1] = 0.0;
        return 0;
    }
    set_lambda(lambda);
    double Ph = HE->P(Vh,Th);
    if( isnan(Ph) )
    {
        //cout << "debug: P isnan\n";
        return 1;
    }    
    double eh = HE->e(Vh,Th);
    //
    double cT2   = HE->cT2(Vh,Th);
    double Gamma = HE->Gamma(Vh,Th);
    double Cv    = HE->CV(Vh,Th);
    // 1 -> V  and 2->T
    double A11 = m2 - cT2/(Vh*Vh);
    double A12 = (Gamma/Vh)*Cv;
    double A21 = A12*Th;
    double A22 = Cv;
    double det = A11*A22 - A12*A21; // Cv*[m^2-(rho*cs)^2] < 0
    //
    // fix for singularity at CJ state
    if( lambda > lambda_CJ && -det < 0.075*Cv*m2 )
    {
        //yp_z[0] = 0.0;
        //yp_z[1] = 0.0;
        //return 0;
        det = -0.075*Cv*m2;
    }
    //
    double Vp  = HE->V2;
    double hp  = products->e(Vp,Th)+Ph*Vp;
    double Kp  = products->KT(Vp,Th);
    double Gp  = products->Gamma(Vp,Th);
    double Cvp = products->CV(Vp,Th);
    //
    double Vr  = HE->V1;
    double hr  = reactants->e(Vr,Th)+Ph*Vr;
    double Kr  = reactants->KT(Vr,Th);
    double Gr  = reactants->Gamma(Vr,Th);
    double Cvr = reactants->CV(Vr,Th);
    //
    // S1 = (dP/dlambda)_V,T > 0
    // S2 = (de/dlambda)_V,T < 0
    double den = lambda*Vp/Kp+(1.-lambda)*Vr/Kr;
    double S1 = (Vp-Vr)/den;  
    double S2 = (hp-hr) - (lambda*Gp*Cvp/Kp+(1.-lambda)*Gr*Cvr/Kr)*Th*S1;
    // correct cumulative integration errors
    double dP = Ph - (P0 + m2*(V0-Vh));
    double de = eh - (e0+0.5*(Ph+P0)*(V0-Vh));
    //S1 *= 1.+ 10.*dP*Vh/cT2;
    //S2 *= 1.- de/(Cv*Th);
    S1 += 100.*dP;
    S2 += 100.*de;
    //
    yp_z[0] = -(S1*A22-S2*A12)/det;  // dVh/dlambda
    yp_z[1] = -(A11*S2-A21*S1)/det;  // dTh/dlambda
    return 0;
}
int DefProfile::refine(double lambda, double &Vh, double &Th)
{
    // One Newton-Rapheson iteration
    if( lambda <= 0.0 )
        return 0;
    else if( lambda >= 1.-1.e-10 )
        lambda = 1.-1.e-10;
    set_lambda(lambda);
    double Ph = HE->P(Vh,Th);
    double eh = HE->e(Vh,Th);
    //
    double cT2   = HE->cT2(Vh,Th);
    double Gamma = HE->Gamma(Vh,Th);
    double Cv    = HE->CV(Vh,Th);
    // 1 -> V  and 2->T
    double A11 = m2 - cT2/(Vh*Vh);
    double A12 = (Gamma/Vh)*Cv;
    double A21 = A12*Th;
    double A22 = Cv;
    //
    double dP = Ph - (P0 + m2*(V0-Vh));
    double de = eh - (e0+0.5*(Ph+P0)*(V0-Vh));
    //
    double det = A11*A22 - A12*A21; // Cv*[m^2-(rho*cs)^2] < 0
    if( lambda > 0.999 )
    {   // fix for singularity at CJ state
        double det_min = 1e-6*Cv*m2;
        if( -det < det_min )
            det = -det_min;
    }
    Vh -= (dP*A22-de*A12)/det;
    Th -= (A11*de-A21*dP)/det;
    return 0;
}
double DefProfile::MaxNorm(const double *y0, const double *y1,
                        const double*, const double*)
{
    double Norm = 0;
    double eps[2] = {1e-3,1.};
    for(int i=Dimension(); i--; y0++, y1++)
    {
        eps[i] *= epsilon;
        Norm = max(Norm, abs(*y0 - *y1)/((abs(*y0)+abs(*y1) + 1.)*eps[i]));
    }
    return Norm;
}
//
//
//
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file     = "HE.data";
    const char *type     = "HEburn";
    const char *name     = "PBX9501.BM";
    const char *material = NULL;
    const char *units    = NULL;
    const char *lib      = "../lib/Linux";
    double epsilon = 1.e-6;     // ODE tolerance
    double dlambda_max = 0.0;
    int count  = 0;             // Newton-Raphson iteration to correct (V,T)
    int nsteps = 20;            // number of steps for printout of profile
    int nlast  = 0;             // last number of steps near lambda=1
    //
    //double V0 = 0.0;            // initial V
    //double e0 = 0.0;            // initial e
    double Ps = 0.0;            // initial shock pressure
    double D  = 0.0;            // Deflagration speed
    int debug = 0;              // flag for diagnostic print out
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        //GetVar(type,type);
        GetVar(material,material);
        GetVar(lib,lib);
        GetVar(units,units);
        GetVar(epsilon,epsilon);
        GetVar(dlambda_max,dlambda_max);
        GetVar(count,count);
        GetVar(nsteps,nsteps);
        GetVar(nlast,nlast);
        //
        //GetVar(V0,V0);
        //GetVar(e0,e0);
        GetVar(Ps,Ps);
        GetVar(D,D);
        GetVar(debug,debug);
        ArgError;
    }
    //
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
    //
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
    if( material )
    {
        if( TypeName(material,type,name) )
            cerr << Error("syntax error, material = ") << material
                 << Exit;
        if( strcmp(type,"HEburn") )
            cerr << Error("material type not HEburn") << Exit;
    }
    if( name==NULL )
        cerr << Error("must specify name or material") << Exit;
    if( lib )
#ifdef LINUX
      setenv("SharedLibDirEOS",lib,1);
#else
      putenv(Cat("SharedLibDirEOS=",lib));
#endif
    //
    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    HEburn *HE = dynamic_cast<HEburn *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    EOS *Reactants = HE->Reactants();
    if( Reactants == NULL )
        cerr << Error("Reay[4]ctants is NULL") << Exit;
    EOS *Products = HE->Products();
    if( Products == NULL )
        cerr << Error("Products is NULL") << Exit;
    Arrhenius *Rate =  dynamic_cast<Arrhenius *>(HE->rate);
    if( Rate == NULL )
        cerr << Error("HE->rate not Arrhenius") << Exit;
    //
    double Vref = HE->V_ref; 
    double eref = HE->e_ref;
    double Pref = HE->P(Vref,eref);
    double Tref = HE->T(Vref,eref);
    HydroState state0(Vref,eref);
    //
    if( Ps <= Pref )
        cerr << Error("Ps <= Pref") << Exit;
    Hugoniot *hug = Reactants->shock(state0);
    if( hug==NULL )
        cerr << Error("Reactants->shock failed" ) << Exit;
    WaveState S_state;              // shock state
    if( hug->P(Ps,RIGHT,S_state) )
        cerr << Error("hug->P failed") << Exit;
    WaveState A_state = S_state;    // state ahead of deflagration
    A_state.u = 0.0;                // rest frame of state behind shock   
    Deflagration *defl =  Products->deflagration(A_state,A_state.P);
    if( defl==NULL )
        cerr << Error("Products->deflagration failed" ) << Exit;
    WaveState CJ_state;
    if( defl->CJwave(RIGHT,CJ_state) )
         cerr << Error("CJwave failed") << Exit;
    DefProfile profile(Rate);
    profile.epsilon = epsilon;
    if( dlambda_max > 0.0 )
        profile.dt_max = dlambda_max;
    double lambda0 = 0.0;
    double V0 = A_state.V;
    double e0 = A_state.e;
    double T0 = Reactants->T(A_state.V,A_state.e);
    double P0 = Reactants->P(A_state.V,A_state.e);
    double u0 = A_state.u;
    WaveState D_state;  // state behind deflagration wave
    if( defl->u_s(D,RIGHT,D_state) )
         cerr << Error("defl_us failed") << Exit;
    double Tdefl = Products->T(D_state.V,D_state.e);
    
    // print header
    cout << type << "::" << name << "\n";
    PrintState(*Reactants, S_state) << " shock state\n";
    PrintState(*Reactants, A_state) << " ahead state\n";
    PrintState(*Products, CJ_state) << "    CJ state\n";
    PrintLine();
    PrintLabels();
    PrintUnits(*eos);
    PrintLine();
    PrintState(*Products, D_state) << " deflagration\n";
    int status = profile.Init(lambda0,V0,T0,D,u0);
    if( status )
        cerr << Error("Profile.init failed with status ")
             << profile.ErrorStatus(status) << Exit;
    cout.setf(ios::left, ios::adjustfield);
    cout << "Deflagration speed = " << D << "\n";
    cout        << setw(11) << "#  lambda"
         << " " << setw(11) << "  dlambda"
         << " " << setw(11) << "   V"
         << " " << setw(11) << "   T"
         << " " << setw(11) << "   P"
         << " " << setw(11) << "   dP/P"
         << " " << setw(11) << "   e"
         << " " << setw(11) << "   de/e"
         << " " << setw(11) << "   u"
         << " " << setw(11) << "   cT"
         << " " << setw(11) << "  (D-u)/cT"
         << " " << setw(11) << " dV/dlambda"
         << " " << setw(11) << " dT/dlambda"
         << "\n";
    cout.setf(ios::right, ios::adjustfield);
    double lambda,  y[2], yp[2];
    double u, T, V, P, e;
    double dP, de;
    int i;
    int k = nlast;
    double cfact = 1.0;
    for( i=0; i<=nsteps; i++ )
    {
        if( i == nsteps && k > 0 )
        {
           if( k-- == 0 )
              return 0;
           lambda = 1. -(1.-lambda)/10.;
           nsteps++;
           cfact = sqrt(1.-lambda);
        }
        else
            lambda = lambda0 +double(i)*(1.-lambda0)/double(nsteps);
        status = profile.Integrate(lambda,y,yp);
        if( status )
        {
            profile.LastState(lambda,y,yp);            
            cerr << Error("Integrate failed with status ")
                 << profile.ErrorStatus(status) << "\n";
            cerr << "Last state: lambda,V,T = " << lambda
                 << " " << y[0] << " " << y[1] << "\n";
            if( i < nsteps )
                return 1;
        }
        V = y[0];
        T = y[1];
        int j = count;
        if( k<nlast)
            j *= 2;
        while( j-- )
            profile.refine(lambda,V,T);
        profile.Fext(yp,y,lambda,1.0);
        P = profile.P(lambda,V,T);
        e = profile.e(lambda,V,T);
        u = D - profile.m*V;
        dP = P - (P0 + profile.m2*(V0-V));
        de = e - (e0+0.5*(P+P0)*(V0-V));
        double dlambda = profile.StepSize();
        double cT2   = profile.HE->cT2(V,T);
        cout        << setw(11) << setprecision(4) << lambda
             << " " << setw(11) << setprecision(4) << dlambda
             << " " << setw(11) << setprecision(4) << V
             << " " << setw(11) << setprecision(4) << T
             << " " << setw(11) << setprecision(4) << P
             << " " << setw(11) << setprecision(4) << dP/P
             << " " << setw(11) << setprecision(4) << e
             << " " << setw(11) << setprecision(4) << de/e
             << " " << setw(11) << setprecision(4) << u
             << " " << setw(11) << setprecision(4) << sqrt(cT2)
             << " " << setw(11) << setprecision(4) << (D-u)/sqrt(cT2)
             << " " << setw(11) << setprecision(4) << cfact*yp[0]
             << " " << setw(11) << setprecision(4) << cfact*yp[1]
             << "\n";
/******
        double c2 = profile.HE->c2(V,T);
        cout << "   u,c= " << u << " " << sqrt(c2);
        cout << "    u+c = " << u+sqrt(c2) << "\n";
******/
/*********************
        // test: reaction
        //       followed by viscosity equilibrating pressure to Rayleigh line
        //       followed by heat flux to adjust temperature
        double lambda1 = lambda+ 0.01;
        if( lambda1 >= 1.0 )
            continue;
        // loop for P(V1,T,lambda1) on Rayleigh line
        double V1 = V;
        double P1;
        int niter = 10;
        while( niter-- )
        {
           P1 = profile.P(lambda1,V1,T);
           dP =  P1 - (P0 + profile.m2*(V0-V1));
           if( abs(dP) < 1e-6*P1 )
               break;
           double cT2 = profile.HE->cT2(V1,T);
           V1 += dP/(cT2/(V1*V1)-profile.m2);
        }
        if( niter < 0 )
        {
            cout << "V1 failed to converge\n";
            continue;
        }
        //
        double e1 = profile.e(lambda1,V1,T);
        de = e1 - (e0+0.5*(P1+P0)*(V0-V1));
        cout << "   de/e1= " << de/e1 << " dP/P= " << dP/P1 << "\n";
******************/
    }
    delete defl;
    delete hug;
    deleteEOS(Products);
    deleteEOS(Reactants);
    deleteEOS(eos);
    return 0;
}
