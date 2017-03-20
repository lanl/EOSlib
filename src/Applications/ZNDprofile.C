#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include <ExtEOS.h>
#include <HEburn.h>
//
class ZNDprofile : public ODE
{
// xi = x - D*t <= 0, right facing wave
// m = (D-u)*rho                // mass flux = D*rho0, for u0=0
// D-u = m*V
// (d/dxi) M = -1/V             // mass coordinate
// (d/dxi) t = -1/(D-u)         // Lagrangian time
// (D-u) * (d/dxi) z[i]  = -Rate[i], for i = 0, ..., N
// lambda = z[ilambda]
public:
    ExtEOS &HE;
    int ilambda;     // Z index of lambda
    int N;           // HE.N() = number of IDOF, z[i]
                     // assumes z[0] = lambda
    WaveState ws;    // wave state behind lead shock (VN spike state)    
    double D;        // w0.us, wave velocity
    double V0;       // ahead V
    double e0;       // ahead e
    double P0;       // ahead P
                     // ahead state at rest, u0=0
    double m;        // rho0*D, shock strength
    double m2;       // (rho0*D)^2

    double *y, *yp;  // ODE variables, yp = (d/dxi)y
                     // y[0] = M, mass behind VN spike
                     // y[1] = t, reaction time behind VN spike
                     // y[2+i] = HE.z[i], i=0,...,N
    double *z;       // y+2;
    double *zp;      // yp+2;
    ZNDprofile(ExtEOS &he, const WaveState &w0, const WaveState &wvn);
    int F(double *y_prime, const double *y_xi, double xi); // ODE::F 
    int Lambda(double &lambda, double &xi, double &V, double &e,
                                           double &P, double &u);
    void Last(double &xi, double &V, double &e, double &P, double &u);
    using ODE::epsilon;
    double abs_tol;     // for ODEfunc
    double rel_tol;     // for ODEfunc    
private:
    // state on partly burned locus with mass flux m
    void LocusState(const double *z, double &V, double &e,
                                     double &P, double &u);
    double Vlast;
public:
    int maxcount;       // DEBUG
    int sumcount;       // DEBUG
};
ZNDprofile::ZNDprofile(ExtEOS &he, const WaveState &w0, const WaveState &wvn)
                      : ODE(2+he.N(),512), HE(he), ws(wvn)
{
    epsilon = 1.e-10;
    abs_tol = 1.e-14;
    rel_tol = 1.e-8;

    N = HE.N();
    D = ws.us;
    V0 = w0.V;
    e0 = w0.e;
    P0 = w0.P;
    m  = D/V0;
    m2 = m*m;
    Vlast = ws.V;

    if( N == 1 )
        ilambda = 0;
    else
    {
        ilambda = HE.Zref()->var_index("lambda");
        if( ilambda < 0 )
            cerr << Error("ilambda < 0") << Exit;
    }
    y  = new double[N+2];
    z  = y+2;
    yp = new double[N+2];
    zp = yp+2;
    y[0] = 0.0;
    y[1] = 0.0;
    double *z0 = &(*HE.Zref());
    for( int i=0; i<N; i++ )
        z[i] = z0[i];
    double dt;
    int status = HE.TimeStep(ws.V,ws.e,z,dt);
    if( status < 1 )
        cerr << Error("TimeStep failed") << Exit;
    //
    double dxi = ws.us*dt;
    double xi0 = 0.0;
    if( (status=Initialize(xi0, y, -dxi)) )
        cerr << Error("ZNDprofile, Initialize failed with status ")
             << ErrorStatus(status) << Exit;
    maxcount = 0;       // DEBUG
    sumcount = 0;       // DEBUG
}

void ZNDprofile::LocusState(const double *z, double &V, double &e,
                                             double &P, double &u)
{
    V = Vlast;
    int count = 20;
    while( count-- )
    {
       double dV = V0-V;
       e = e0 + 0.5*m2*dV*dV + P0*dV;
       P = HE.P(V,e,z);
       double h = e-e0 - 0.5*(P+P0)*(V0-V);
       if( std::abs(h) < 1e-12*(P+P0)*(V0-V) ) break;

       double c2     = HE.c2(V,e,z);
       double GammaV = HE.Gamma(V,e,z) /V;
       double f  = P-P0 - m2*(V0-V);
       double dfdV = -c2/(V*V) + GammaV*(P-P0 - m2*(V0-V)) + m2;
       // update
       V -= f/dfdV;
       V = min(V0, max(ws.V ,V) );
    }
    u = sqrt( (P-P0)*(V0-V) );
    Vlast = V;

    maxcount = max(maxcount, 20-count);     // DEBUG
    sumcount += 20-count;
}

int ZNDprofile::F(double *y_prime, const double *y_xi, double xi)
{
    const double *z_xi  = y_xi+2;
          double *zp = y_prime+2;

    double V,e,P,u;
    LocusState(z_xi, V,e,P,u);
    double du = m*V;

    y_prime[0] = -1./V;                     // M
    y_prime[1] = -1./du;                    // t
    if( HE.Rate(V,e,z_xi, zp) != N )
        return 1;
    for( int i=0; i<N; ++i )
    {
        zp[i] *= -1./du;
    }
/****
cout << "lambda,V,e,P,u " << z_xi[0]
     << " " << V << " " << e << " " << P << " " << u << "\n";
cout << "yp " << y_prime[0] << " " << y_prime[1]
       << " " << y_prime[2] << "\n";
****/
    return 0;
}
class lambda_ODE : public ODEfunc
{
    int ilambda;
public:
    lambda_ODE(int i) { ilambda=i; }
    double f(double xi, const double *y, const double *yp);
};
double lambda_ODE::f(double, const double *y, const double *yp)
{
    return -y[2+ilambda];   // -lambda
}
int ZNDprofile::Lambda(double &lambda, double &xi, double &V, double &e,
                                                   double &P, double &u)
{
    lambda_ODE L(ilambda);
    lambda = -lambda;       // -lambda(xi)is monotonically increasing 
    int status = Integrate(L,lambda,xi,y,yp);
    lambda = -lambda;
    if( status )
        return status;

    double du = -1./yp[1];
    V = du/m;
    // Correct shock state
    LocusState(z,V,e,P,u);

    return 0;    
}
void ZNDprofile::Last(double &xi, double &V, double &e, double &P, double &u)
{
    LastState(xi,y,yp);
    double du = -1./yp[1];
    V = du/m;
    LocusState(z,V,e,P,u);
}

//
//
const char *help[] = {    // list of commands printed out by help option
    "name        name    # EOS name",
    "type        name    # EOS type",
    "material    name    # type::name",
    "file[s]     file    # colon separated list of data files",
    "lib         name    # directory for EOSlib shared libraries",
    "                    # default environ variable EOSLIB_SHARED_LIBRARY_PATH",
    "units       name    # default units from data base",
    "",
    "upiston     num     # particle velocity behind detonation wave",
    "nsteps      int     # number of points in each stage of profile",
    "",
    "epsilon     num     # ODE tolerance",
    "abs_tol     num     # ODEfunc tolerance",
    "rel_tol     num     # ODEfunc tolerance",  
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
//  ZNDprofile
//
int main(int, char **argv)
{
    ProgName(*argv);

    const char *files    = NULL;
    const char *lib      = NULL;

    const char *type     = "HEburn2";
    const char *name     = NULL;
    const char *material = NULL;
    const char *units    = NULL;

    double epsilon = 1.e-8;    // ODE tolerance
    double abs_tol = 1.e-14;   // ODEfunc tolerance
    double rel_tol = 1.e-8;    // ODEfunc tolerance

    int printddt   = 0;

    double upiston = 0;
    int nsteps = 20;
    int finish = 0;

    while(*++argv)
    {
        GetVar(file, files);
        GetVar(files,files);
	    GetVar(lib,lib);

        GetVar(name,name);
        GetVar(type,type);
	    GetVar(material,material);
        GetVar(units,units);

        GetVar(upiston,upiston);
        GetVar(nsteps,nsteps);

        GetVar(epsilon,epsilon);
        GetVar(abs_tol,abs_tol);
        GetVar(rel_tol,rel_tol);

        if( !strcmp(*argv, "?") || !strcmp(*argv,"help") )
            Help(0);
        ArgError;
    }
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
	if( material )
	{
	    if( TypeName(material,type,name) )
		    cerr << Error("syntax error, material = ") << material
		         << Exit;
	}
    if( name==NULL )
        cerr << Error("must specify (HEburn2::)name") << Exit;
    //
    DataBase db;
    if( db.Read(files) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    ExtEOS *HE = dynamic_cast<ExtEOS *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    {
        // if HEburn then set tolerances PT equilibrium
        HEburn *heburn = dynamic_cast<HEburn *>(HE);
        if( heburn )
        {
            heburn->HE->tol_P = 1e-12;
            heburn->HE->tol_T = 1e-4;
            heburn->HE->lambda1_max = 0.99;
            heburn->HE->lambda2_max = 0.99;
            heburn->HE->max_iter  = 100;
            heburn->HE->max_iter1 = 100;
        }
    }
    //
    double Vref = eos->V_ref; 
    double eref = eos->e_ref;
    HydroState state0(Vref,eref);
    WaveState  WS0;
    HE->Evaluate(state0, WS0);
    double Pref = WS0.P;
    double cref = WS0.us;
    double Tref = HE->T(Vref,eref);
    double Sref = HE->S(Vref,eref);    
    //
    Hugoniot   *hug = HE->shock(state0);
    Detonation *det = HE->detonation(state0,Pref);
    if( hug==NULL || det == NULL )
        cerr << Error("HE->detonation failed" ) << Exit;
    WaveState CJ;
    if( det->CJwave(RIGHT,CJ) )
        cerr << Error("det->CJwave failed" ) << Exit;
    //
    upiston = max(upiston,CJ.u);
    WaveState OD;   // overdriven detonation
    if( det->u(upiston, RIGHT,OD) )
        cerr << Error("det->u failed" ) << Exit;
    cout << "# ZNDprofile for " << type << "::" << name << "\n";
    cout << "# D = " << OD.us << "\n";
    //
    WaveState VN;
    if( hug->u_s(OD.us,RIGHT,VN) )
        cerr << Error("hug->u_s failed") << Exit;
    delete det;
    delete hug;
    //
    // ZND profile
    //
    ZNDprofile profile(*HE,WS0,VN);
    // IG model rate has discontinuity
    //    large initiation rate to small growth rate at small lambda
    //    ODE tolerance requires very small step size to get past discontinuity
           profile.dt_min =1e-14;
    // or increasing tolerance epsilon <= 1e-7
    profile.epsilon = epsilon;
    profile.abs_tol = abs_tol;
    profile.rel_tol = rel_tol;
    double *y   = profile.y;
    double *z   = profile.z;
    double *yp  = profile.yp;
    double *zp  = profile.zp;
    int N       = profile.N;
    int ilambda = profile.ilambda;
    double xi,t,M, lambda;
    double V, e,de, u, c, P, T, S;
    int i;

	cout.setf(ios::left, ios::adjustfield);
    cout        << setw(15) << "# t"
         << " " << setw(15) << "   xi"
         << " " << setw(15) << "   M"
         << " " << setw(15) << "   V"
         << " " << setw(15) << "   e"
         << " " << setw(15) << "   u"
         << " " << setw(15) << "   c"
         << " " << setw(15) << "   P"
         << " " << setw(15) << "   T"
         << " " << setw(15) << "   S"
         << " " << setw(15) << "   lambda"
         << "\n";
    const Units *U = eos->UseUnits();
    cout << "# " << setw(15) << U->Unit("t")
         << "  " << setw(15) << U->Unit("L")
         << " "  << setw(15) << "rho*len"
         << " "  << setw(15) << U->Unit("V")
         << " "  << setw(15) << U->Unit("e")
         << " "  << setw(15) << U->Unit("u")
         << " "  << setw(15) << U->Unit("u")
         << " "  << setw(15) << U->Unit("P")
         << " "  << setw(15) << U->Unit("T")
         << " "  << setw(15) << U->Unit("S")
         << " "  << setw(15) << " "
         << "\n";
	cout.setf(ios::showpoint);
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::scientific, ios::floatfield);
    //
    // Ambient state
    //
    xi = 0.1;
    t  = -xi/OD.us;
    M  = xi/OD.V;
    V  = Vref;
    e  = eref;
    u  = 0.0;
    c  = cref;
    P  = Pref;
    T  = Tref;
    S  = Sref;
    lambda = 0.0;
    for( i=2; i; i-- )
    {
        cout        << setw(15) << setprecision(8) << t
             << " " << setw(15) << setprecision(8) << xi
             << " " << setw(15) << setprecision(8) << M
             << " " << setw(15) << setprecision(8) << V
             << " " << setw(15) << setprecision(8) << e
             << " " << setw(15) << setprecision(8) << u
             << " " << setw(15) << setprecision(8) << c
             << " " << setw(15) << setprecision(8) << P
             << " " << setw(15) << setprecision(8) << T
             << " " << setw(15) << setprecision(8) << S
             << " " << setw(15) << setprecision(8) << lambda
             << "\n";
        t  = 0.0;
        xi = 0.0;
        M  = 0.0;
    }
    //
    // reaction profile
    //
    for( i=0; i<=nsteps; i++ )
    {
        lambda = double(i)/double(nsteps);
        lambda = min(lambda, 1.-1.e-12);
        // integrate to specified values of lambda
        int status = profile.Lambda(lambda,xi, V,e,P,u);
        if( status )
        {
            profile.Last(xi,V,e,P,u);
            // TIME_STEP_ERROR due to dlambda/dxi=0 at end of first reaction
            if( 1.0 - lambda < 1e-10  )
            {
                lambda = 1.0;
                status  = 0;
            }
        }
        M = -y[0];
        t =  y[1];

        P = HE->P(V,e,z);
        T = HE->T(V,e,z);
        c = HE->c(V,e,z);
        S = HE->S(V,e,z);
        cout        << setw(15) << setprecision(8) << t
             << " " << setw(15) << setprecision(8) << xi
             << " " << setw(15) << setprecision(8) << M
             << " " << setw(15) << setprecision(8) << V
             << " " << setw(15) << setprecision(8) << e
             << " " << setw(15) << setprecision(8) << u
             << " " << setw(15) << setprecision(8) << c
             << " " << setw(15) << setprecision(8) << P
             << " " << setw(15) << setprecision(8) << T
             << " " << setw(15) << setprecision(8) << S
             << " " << setw(15) << setprecision(8) << lambda
             << "\n";
        if( status )
            cerr << Error("profile.Lambda failed with status ")
                 << profile.ErrorStatus(status) << Exit;
    }
    deleteEOS(eos);
    return 0;
}
