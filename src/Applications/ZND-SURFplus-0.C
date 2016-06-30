#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include <HEburn2.h>
#include <HotSpotRateCC.h>
//
class ZNDprofile : public ODE
{
// xi = x - D*t <= 0, right facing wave
// (d/dxi) M =  1/V             // mass coordinate
// (d/dxi) t = -1/(D-u)         // Lagrangian time
// [c^2-(D-u)^2] (d/dxi) u = V*[(\partialP/\partial\lambda1)*Rate_1
//                              +(\partialP/\partial\lambda2)*Rate_2]
// (D-u) * (d/dxi) s1 = -Rate_s1
// (D-u) * (d/dxi) s2 = -Rate_s2
// Rate_1 = g'(s1)*Rate_s1
// Rate_2 =  2*s2 *Rate_s2
public:
    HEburn2 &HE;
    HotSpotRateCC *Rate;    // derived type HErate2
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
                     // z[0] = Ps
                     // z[1] = t-(ts=0)
                     // z[2] = s1
                     // z[3] = s2
    ZNDprofile(HEburn2 &he, WaveState &w);
    int F(double *y_prime, const double *y_xi, double xi); // ODE::F 
    int Lambda1(double &lambda, double &xi, double &V, double &e, double &P);
    int Lambda2(double &lambda, double &xi, double &V, double &e, double &P);
    void   Last(                double &xi, double &V, double &e, double &P);
};
ZNDprofile::ZNDprofile(HEburn2 &he, WaveState &w)
        : ODE(he.N()+3,512), HE(he), ws(w)
{
    Rate = dynamic_cast<HotSpotRateCC *>(HE.rate);
    if( Rate == NULL )
    {
        cerr << Error("ZNDprofile, dynamic_cast failed") << Exit;
    }
    N = HE.N();
    D = ws.us;
    double du = D-ws.u;     //  paticle velocity relative to wave front
    mflux = du/ws.V;
    B = ws.e + ws.P*ws.V + 0.5*du*du;
    int i = HE.Zref()->var_index("lambda");
    y  = new double[N+3];
    z  = y+3;
    yp = new double[N+3];
    zp = yp+3;
    y[0] = 0.0;     // M
    y[1] = ws.u;    // u
    y[2] = 0.0;     // t
    z[0] = ws.P;    // Ps
    z[1] = 0.0;     // t - (ts=0)
    z[2] = 0.0;     // s1
    z[3] = 0.0;     // s2
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

    const double *z_xi  = y_xi+3;
          double *zp = y_prime+3;

    double s1 = z_xi[2];
    double s2 = z_xi[3];
    double lambda1 = Rate->g(s1);
    double lambda2 = s2*s2;
    if( lambda2 >= 1. )
    {
        int i;
        for( i=0; i<N+3; i++ )
            y_prime[i] = 0.0;
        return 0;
    }
    double V = du/mflux;
    double P = ws.P - mflux*(ws.u-u);
    double e = B - (P*V+0.5*du*du);


    double c2 = HE.c2(V,e,z_xi);
    if( std::isnan(c2) || c2 <= du*du )
        return 1;

    if( Rate->Rate(V,e,z_xi, zp) )
        return 1;

    double dP1, dP2;
    if( Rate->dPdlambda(V,e,lambda1,lambda2,dP1,dP2) )
        return 1;
    double sigma1 = dP1*V/c2;
    double sigma2 = dP2*V/c2;

    double rate_l1 = Rate->dgds(s1)*zp[2];  // dlambda1/dt
    double rate_l2 = 2.*s2*zp[3];           // dlambda2/dt
    double sigmaRate = sigma1*rate_l1 + sigma2*rate_l2;

    y_prime[0] = -1./V;                     // M
    y_prime[1] =  c2*sigmaRate/(c2-du*du);  // u
    y_prime[2] = -1./du;                    // t
    y_prime[3] = 0.0;                       // Ps
    y_prime[4] = y_prime[2];                // ts
    y_prime[5] = -zp[2]/du;                 // s1
    y_prime[6] = -zp[3]/du;                 // s2

    return 0;
}

class lambda1_ODE : public ODEfunc
{
    HotSpotRateCC *Rate;
public:
    lambda1_ODE(HotSpotRateCC *rate) { Rate=rate;}
    double f(double V, const double *y, const double *yp);
};
double lambda1_ODE::f(double, const double *y, const double *yp)
{
    double s1 = y[5];
    return -Rate->g(s1);   // -lambda1
}
int ZNDprofile::Lambda1(double &lambda, double &xi,
                                       double &V, double &e, double &P)
{
    lambda1_ODE L(Rate);
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

class lambda2_ODE : public ODEfunc
{
public:
    lambda2_ODE() { }
    double f(double V, const double *y, const double *yp);
};
double lambda2_ODE::f(double, const double *y, const double *yp)
{
    double s2 = y[6];
    return -s2*s2;   // -lambda2
}
int ZNDprofile::Lambda2(double &lambda, double &xi,
                                       double &V, double &e, double &P)
{
    lambda2_ODE L;
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

void ZNDprofile::Last(double &xi, double &V, double &e, double &P)
{
    LastState(xi,y,yp);
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
    const char *file     = "HE.data";;
    const char *type     = "HEburn2";
    const char *name     = "PBX9502.HotSpotCC";
	const char *material = NULL;
    const char *units    = NULL;
    const char *lib      = NULL;
    double epsilon = 1.e-9;

    double upiston = 0.;
    double umin    = 0.;    // velocity at end of Taylor Wave
    double len     = 10.;   // length of run for  Taylor Wave
    int nsteps = 20;
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
        ArgError;
    }
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
	if( material )
	{
	    if( TypeName(material,type,name) )
		    cerr << Error("syntax error, material = ") << material
		         << Exit;
        if( strcmp(type,"HEburn2") )
            cerr << Error("material type not HEburn2") << Exit;
	}
    if( name==NULL )
        cerr << Error("must specify (HEburn2::)name") << Exit;
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
    HEburn2 *HE = dynamic_cast<HEburn2 *>(eos);
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
    double D = OD.us;   // detonation speed
    cout << "# ZNDprofile + Taylor wave for " << type << "::" << name << "\n";
    cout << "# D = " << D << "\n";
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
    double xi,t,M, lambda1,lambda2;
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
         << " " << setw(13) << "   S"
         << " " << setw(13) << "   lambda1"
         << " " << setw(13) << "   lambda2"
         << "\n";
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
    lambda1 = 0.0;
    lambda2 = 0.0;
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
             << " " << setw(13) << setprecision(6) << S
             << " " << setw(13) << setprecision(6) << lambda1
             << " " << setw(13) << setprecision(6) << lambda2
             << "\n";
        t  = 0.0;
        xi = 0.0;
        M  = 0.0;
    }
    for( i=0; i<=nsteps; i++ )
    {
        lambda1 = double(i)/double(nsteps);
        // integrate to specified values of lambda
        int status = profile.Lambda1(lambda1,xi, V,e,P);
        if( status )
            profile.Last(xi,V,e,P);
        M = -y[0];
        u =  y[1];
        t =  y[2];
        T = HE->T(V,e,z);
        c = HE->c(V,e,z);
        S = HE->S(V,e,z);
        lambda2 = z[3]*z[3];
        cout        << setw(13) << setprecision(6) << t
             << " " << setw(13) << setprecision(6) << xi
             << " " << setw(13) << setprecision(6) << M
             << " " << setw(13) << setprecision(6) << V
             << " " << setw(13) << setprecision(6) << e
             << " " << setw(13) << setprecision(6) << u
             << " " << setw(13) << setprecision(6) << c
             << " " << setw(13) << setprecision(6) << P
             << " " << setw(13) << setprecision(6) << T
             << " " << setw(13) << setprecision(6) << S
             << " " << setw(13) << setprecision(6) << lambda1
             << " " << setw(13) << setprecision(6) << lambda2
             << "\n";
        if( status )
            cerr << Error("profile.Lambda1 failed with status ")
                 << profile.ErrorStatus(status) << Exit;
    }
    double lambda = lambda2;
    for( i=0; i<=nsteps; i++ )
    {
        lambda2 = double(i)/double(nsteps);
        if( lambda2 <= lambda ) continue;
        // integrate to specified values of lambda
        int status = profile.Lambda2(lambda2,xi, V,e,P);
        if( status )
            profile.Last(xi,V,e,P);
        M = -y[0];
        u =  y[1];
        t =  y[2];
        T = HE->T(V,e,z);
        c = HE->c(V,e,z);
        S = HE->S(V,e,z);
        lambda2 = z[3]*z[3];
        cout        << setw(13) << setprecision(6) << t
             << " " << setw(13) << setprecision(6) << xi
             << " " << setw(13) << setprecision(6) << M
             << " " << setw(13) << setprecision(6) << V
             << " " << setw(13) << setprecision(6) << e
             << " " << setw(13) << setprecision(6) << u
             << " " << setw(13) << setprecision(6) << c
             << " " << setw(13) << setprecision(6) << P
             << " " << setw(13) << setprecision(6) << T
             << " " << setw(13) << setprecision(6) << S
             << " " << setw(13) << setprecision(6) << lambda1
             << " " << setw(13) << setprecision(6) << lambda2
             << "\n";
         if( status )
         {
            if( abs(D-(u+c)) < 0.0001*D ) break;
            cerr << Error("profile.Lambda2 failed with status ")
                 << profile.ErrorStatus(status)
                 << "\nD "  <<  setprecision(6) << D
                 << " u+c " << setprecision(6) << u+c
                 << Exit;
         }
    }
    if( len > 0. )
    {
    // Taylor wave from detonation state
      V = OD.V;
      e = OD.e;
      u = OD.u;
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
      for( i=0; i<=nsteps; i++ )
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
           if( i==0 )
           {
               lambda2 = 1.0;
               cout << " " << setw(13) << setprecision(6) << lambda1
                    << " " << setw(13) << setprecision(6) << lambda2;
           }
           cout << "\n";
      }
      delete I;
      deleteEOS(products);
    }
    deleteEOS(eos);
    return 0;
}
