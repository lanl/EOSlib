#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include <HEburn.h>
//
class ZNDprofile : public ODE
{
// xi = x - D*t <= 0, right facing wave
// (d/dxi) M =  1/V             // mass coordinate
// (d/dxi) t = -1/(D-u)         // Lagrangian time
// [c^2-(D-u)^2] (d/dxi) u = V(\partialP/\partial\lambda)*Rate
// (D-u) * (d/dxi) z[i]  = -Rate[i], for i = 0, ..., N
// z[0] = lambda
public:
    ExtEOS &HE;
    int N;           // HE.N() = number of IDOF, z[i]
                     // assumes z[0] = lambda
    WaveState ws;    // wave state behind lead shock
    double D;        // w0.us, wave velocity
    double mflux;    // mass flux, rho*(D-u)
    double B;        // Bernoulli constant, e + PV + 0.5*(D-u)^2
    double *y, *yp;  // ODE variables, yp = (d/dxi)y
                     // y[0] = M, mass behind VN spike
                     // y[1] = u
                     // y[2] = t, reaction time behind VN spike
                     // y[2+i] = HE.z[i], i=0,...,N
    double *z;       // y+3;
    double *zp;      // yp+3;
    //double t1;      // initial induction time
    ZNDprofile(ExtEOS &he, WaveState &w);
    int F(double *y_prime, const double *y_xi, double xi); // ODE::F 
    int Lambda(double &lambda, double &xi, double &V, double &e, double &P);
    void  Last(double &lambda, double &xi, double &V, double &e, double &P);
};
ZNDprofile::ZNDprofile(ExtEOS &he, WaveState &w)
        : ODE(he.N()+3,512), HE(he), ws(w)
{
    N = HE.N();
    D = ws.us;
    double du = D-ws.u;     //  paticle velocity relative to wave front
    mflux = du/ws.V;
    B = ws.e + ws.P*ws.V + 0.5*du*du;
    int i = HE.Zref()->var_index("lambda");
    if( i != 0 )
        cerr << Error("z[0] must be lambda") << Exit;
    y  = new double[N+3];
    z  = y+3;
    yp = new double[N+3];
    zp = yp+3;
    y[0] = 0.0;
    y[1] = ws.u;
    y[2] = 0.0;
    double *z0 = &(*HE.Zref());
    for( i=0; i<N; i++ )
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
}
int ZNDprofile::F(double *y_prime, const double *y_xi, double xi)
{
    double u  = y_xi[1];
    double du = D - u;
    if( du < 0. )
        return 1;
    double lambda = y_xi[3];
    if( lambda >= 1. )
    {
        int i;
        for( i=0; i<N+3; i++ )
            y_prime[i] = 0.0;
        return 0;
    }
    double V = du/mflux;
    double P = ws.P - mflux*(ws.u-u);
    double e = B - (P*V+0.5*du*du);
    const double *z_xi = y_xi+3;
    if( HE.Rate(V,e,z_xi, zp)!=N )
        return 1;
    double c2 = HE.c2(V,e,z_xi);
    if( std::isnan(c2) || c2 <= du*du )
        return 1;
    y_prime[0] = -1./V;
    y_prime[1] = 0.0;
    int i;
    for( i=0; i<N; i++ )
    {
        double dP, dT;
        if( HE.ddz(i,V,e,z_xi,dP,dT) )
            return 1;
        y_prime[1] += dP*zp[i];
    }
    y_prime[1] *= V/(c2-du*du);
    y_prime[2] = -1./max(du,0.001*D);
    for( i=0; i<N; i++ )
        y_prime[3+i] = -zp[i]/du;
    return 0;
}
class lambda_ODE : public ODEfunc
{
public:
    lambda_ODE() {}
    double f(double V, const double *y, const double *yp);
};
double lambda_ODE::f(double, const double *y, const double *yp)
{
    return -y[3];   // -lambda
}
int ZNDprofile::Lambda(double &lambda, double &xi,
                                       double &V, double &e, double &P)
{
    lambda_ODE L;
    lambda = -lambda;   // -lambda(xi)is monotonically increasing 
    int status = Integrate(L,lambda,xi,y,yp);
    lambda = -lambda;
    if( status )
        return status;
    double u = y[1];
    double du = ws.us-u;
    V = du/mflux;
    P  = ws.P +  mflux*(u-ws.u);
    e = B - (P*V+0.5*du*du);
    return 0;    
}
void ZNDprofile::Last(double &lambda, double &xi,
                                      double &V, double &e, double &P)
{
    LastState(xi,y,yp);
    lambda = y[3];
    double u = y[1];
    double du = ws.us-u;
    V = du/mflux;
    P  = ws.P +  mflux*(u-ws.u);
    e = B - (P*V+0.5*du*du);
}
//
//  ZNDprofile with Taylor wave
//
int main(int, char **argv)
{
    ProgName(*argv);
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/HEburnTest.data";
    const char * file = file_.c_str();
    std::string libPath;
    libPath  = (getenv("EOSLIB_SHARED_LIBRARY_PATH") != NULL) ? getenv("EOSLIB_SHARED_LIBRARY_PATH") : "PATH ENV NOT SET!";
    const char * lib     = libPath.c_str();
    //const char *file     = NULL;
    const char *type     = NULL;//"HEburn";
    const char *name     = NULL;//"PBX9502";NULL;
    const char *material = "HEburn::PBX9502";//NULL;
    const char *units    = NULL;
    //const char *lib      = NULL;
    double epsilon = 1.e-6;
    double lambda1 = 0.95;          // refine end of reaction zone
    double lambda_max =  0.9999;    // burn remainder

    double upiston = 0.;
    double umin    = 0.;    // velocity at end of Taylor Wave
    double len     = 10.;   // length of run for  Taylor Wave
    int nsteps = 20;
    int finish = 0;         // Go to Taylor wave if ODE error in profile
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        //GetVar(type,type);
        GetVar(material,material);
	    GetVar(lib,lib);
        GetVar(units,units);
        GetVar(upiston,upiston);
        GetVar(umin,umin);
        GetVar(len,len);
        GetVar(nsteps,nsteps);
        GetVar(epsilon,epsilon);
        GetVar(lambda_max,lambda_max);
        GetVarValue(finish,finish,1);
        ArgError;
    }
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
        cerr << Error("must specify (HEburn::)name") << Exit;
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
    //
    double Vref = eos->V_ref; 
    double eref = eos->e_ref;
    double Pref = HE->P(Vref,eref);
    double Tref = HE->T(Vref,eref);
    double cref = HE->c(Vref,eref);
    double Sref = HE->S(Vref,eref);
    double *zref = &(*HE->Zref());
    HydroState state0(Vref,eref);
    //
    Hugoniot   *hug = HE->shock(state0);
    Detonation *det = HE->detonation(state0,Pref);
    if( hug==NULL || det==NULL )
        cerr << Error("HE->detonation failed" ) << Exit;
    WaveState CJ;
    if( det->CJwave(RIGHT,CJ) )
        cerr << Error("det->CJwave failed" ) << Exit;
    //
    if( upiston < CJ.u )
        upiston = (1.+1.e-6)*CJ.u;
    WaveState OD;   // overdriven detonation
    if( det->u(upiston, RIGHT,OD) )
        cerr << Error("det->u failed" ) << Exit;
    cout << "# ZNDprofileTW for " << type << "::" << name << "\n";
    cout << "# D = " << OD.us << "\n";
    //
    WaveState VN;
    if( hug->u_s(OD.us,RIGHT,VN) )
        cerr << Error("hug->u_s failed") << Exit;
    delete det;
    delete hug;
    ZNDprofile profile(*HE,VN);
    profile.epsilon = epsilon;
    double *y = profile.y;
    double *z = profile.z;
    double *yp = profile.yp;
    double *zp = profile.zp;
    int N     = profile.N;
    double xi,t,lambda,M;
    double V, e, u, c, P, T, S;
    int i,j;
	cout.setf(ios::left, ios::adjustfield);
    cout        << setw(13) << "# t"
         << " " << setw(13) << "   xi"
         << " " << setw(13) << "   M"
         << " " << setw(13) << "   V"
         << " " << setw(13) << "   e"
         << " " << setw(13) << "   u"
         << " " << setw(13) << "   c"
         << " " << setw(13) << "   P"
         << " " << setw(13) << "   T"
         << " " << setw(13) << "   S";
    IDOF *Zref = HE->Zref();
    for( j=0; j<N; j++ )
         cout << "    " << setw(10) << Zref->var_name[j];
    cout << "\n";
	cout.setf(ios::showpoint);
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::scientific, ios::floatfield);
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
    for( i=2; i; i-- )
    {
        cout        << setw(13) << setprecision(6) << t
             << " " << setw(13) << setprecision(6) << xi
             << " " << setw(13) << setprecision(6) << M
             << " " << setw(13) << setprecision(6) << V
             << " " << setw(13) << setprecision(6) << e
             << " " << setw(13) << setprecision(6) << u
             << " " << setw(13) << setprecision(6) << c
             << " " << setw(13) << setprecision(6) << P
             << " " << setw(13) << setprecision(6) << T
             << " " << setw(13) << setprecision(6) << S;
        for( j=0; j<N; j++ )
            cout << " " << setw(13) << setprecision(6) << zref[j];
        cout << "\n";
        t  = 0.0;
        xi = 0.0;
        M  = 0.0;
    }
    for( i=0; i<=nsteps+10; i++ )
    {
        if( i < nsteps && lambda < lambda1 )
            lambda= double(i)/double(nsteps);
        else
            lambda = 1. - 0.5*(1.-lambda);
        if( lambda > lambda_max )
            break;
        // integrate to specified values of lambda
        int status = profile.Lambda(lambda,xi, V,e,P);
        if ( status )
        {
             if( finish )
             {
                profile.Last(lambda,xi, V,e,P);
                i = nsteps + 100;
             }
             else
             {
               cerr << Error("profile.Lambda failed with status ")
                    << profile.ErrorStatus(status) << Exit;
             }
        }
        M = -y[0];
        u =  y[1];
        t =  y[2];
        T = HE->T(V,e,z);
        c = HE->c(V,e,z);
        S = HE->S(V,e,z);
        cout        << setw(13) << setprecision(6) << t
             << " " << setw(13) << setprecision(6) << xi
             << " " << setw(13) << setprecision(6) << M
             << " " << setw(13) << setprecision(6) << V
             << " " << setw(13) << setprecision(6) << e
             << " " << setw(13) << setprecision(6) << u
             << " " << setw(13) << setprecision(6) << c
             << " " << setw(13) << setprecision(6) << P
             << " " << setw(13) << setprecision(6) << T
             << " " << setw(13) << setprecision(6) << S;
        for( j=0; j<N; j++ )
            cout << " " << setw(13) << setprecision(6) << z[j];
        cout << "\n";
    }
    if( len > 0. )
    {
      HE->Equilibrate(V,e,z);
      // Taylor wave
      EOS *products = HE->Products();
      HydroState ZNDstate(V,e,u);
      double ZND_t  = t;
      double ZND_xi  = xi;
      double ZND_M   = M;
      double ZND_u   = u;
      double ZND_upc = u + c;
      double ZND_rhoc = c/V;
      double t_init  = len/ZND_upc;
      Isentrope *I = products->isentrope(ZNDstate);
      if( I == NULL )
          cerr << Error("products->isentrope failed") << Exit;
      double t0 = t - t_init;
      double upc_last = ZND_upc;
      for( i=1; i<=nsteps; i++ )
      {
          u = (double(nsteps-i)*ZNDstate.u + double(i)*umin)/double(nsteps);
          WaveState wave;
          if( I->u(u,RIGHT,wave) )
              cerr << Error("I->u failed") << Exit;
          T = products->T(wave.V,wave.e);
          c = products->c(wave.V,wave.e);
          S = products->S(wave.V,wave.e);
          double dM  = (ZND_rhoc - c/wave.V)*t_init;
          // on particle trajectory
          // (d/dt)u = -(c/t) [d/d(u+c)]u
          // dt = -t*d(u+c)/c
          t -= (t-t0)*(u+c-upc_last)/c;
          //double dxi = (ZND_upc-(u+c))*t_init;
          // xi = ZND_xi - dxi
          xi = ZND_xi+ZND_u*(t-ZND_t) - (ZND_upc-(u+c))*(t-t0);
          upc_last = u+c;
          cout        << setw(13) << setprecision(6) << t
               << " " << setw(13) << setprecision(6) << xi
               << " " << setw(13) << setprecision(6) << ZND_M  - dM
               << " " << setw(13) << setprecision(6) << wave.V
               << " " << setw(13) << setprecision(6) << wave.e
               << " " << setw(13) << setprecision(6) << u
               << " " << setw(13) << setprecision(6) << c
               << " " << setw(13) << setprecision(6) << wave.P
               << " " << setw(13) << setprecision(6) << T
               << " " << setw(13) << setprecision(6) << S;
          for( j=0; j<N; j++ )
              cout << " " << setw(13) << setprecision(6) << z[j];
          cout << "\n";
      }
      delete I;
      deleteEOS(products);
    }
    deleteEOS(eos);
    return 0;
}
