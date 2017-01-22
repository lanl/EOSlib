#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include <HEburn2.h>
#include <HotSpotRateCC.h>
//
//
//
class ZNDprofile : public ODE
{
// xi = x - D*t <= 0, right facing wave
// m = (D-u)*rho                    // mass flux = D*rho0, for u0=0
// D-u = m*V
// (d/dxi) M =  1/V                 // mass coordinate
// (d/dxi) t = -1/(D-u)             // Lagrangian time
// (D-u) * (d/dxi) s1 = -Rate_s1
// (D-u) * (d/dxi) s2 = -Rate_s2
// Rate_1 = g'(s1)*Rate_s1
// Rate_2 =  2*s2 *Rate_s2
public:
    HEburn2 &HE;
    HotSpotRateCC *Rate;    // derived type HErate2
    int N;           // HE.N() = number of IDOF, z[i]
                     // assumes z[0] = lambda
    WaveState VN;    // start wave state (VN spike state)
    WaveState CJ;    // end   wave state (CJ state)
    double D;        // detonation speed
    double V0;       // ahead V
    double e0;       // ahead e
    double P0;       // ahead P
                     // ahead state at rest, u0=0
    double m;        // rho0*D, shock strength
    double m2;       // (rho0*D)^2

    double *y, *yp;  // ODE variables, yp = (d/dxi)y
                     // y[0] = M, mass behind VN spike
                     // y[1] = t, reaction time behind VN spike
                     // y[1+i] = HE.z[i], i=0,...,N
    double *z;       // y+2;
    double *zp;      // yp+2;
                     // z[0] = Ps
                     // z[1] = ts=0
                     // z[2] = s1
                     // z[3] = s2
    ZNDprofile(HEburn2 &he, const WaveState &w0, const WaveState &vn);
    int F(double *y_prime, const double *y_xi, double xi); // ODE::F 
    int Xi(double &xi, double &V, double &e, double &P, double &u,
                       double &lambda1, double &lambda2);
    int Lambda1(double &lambda, double &xi, double &V, double &e,
                                            double &P, double &u);
    int Lambda2(double &lambda, double &xi, double &V, double &e,
                                            double &P, double &u);
    void   Last(     double &xi, double &V, double &e, double &P, double &u);
    using ODE::epsilon;
    double abs_tol;     // for ODEfunc
    double rel_tol;     // for ODEfunc
    double iter_tol;    // relative tolerance LocusState iteration
    int    iter_max;    // maximum LocusState iterations
private:
    // state on partly burned locus with mass flux m
    // after calling, HE.set_eos(lambda1, lambda2);
    void LocusState(double lambda1, double lambda2,
                    double &V, double &e, double &P, double &u);
    double Vlast;

public:
    int maxcount;       // DEBUG
    int sumcount;       // DEBUG
    int debug;
};
ZNDprofile::ZNDprofile(HEburn2 &he, const WaveState &w0, const WaveState &vn)
        : ODE(2+he.N(),1024), HE(he), VN(vn),
          debug(0), iter_tol(1e-14), iter_max(50)
{
    epsilon = 1.e-10;
    abs_tol = 1.e-14;
    rel_tol = 1.e-8;
    Rate = dynamic_cast<HotSpotRateCC *>(HE.rate);
    if( Rate == NULL )
    {
        cerr << Error("ZNDprofile, dynamic_cast failed") << Exit;
    }
    D = VN.us;
    V0 = w0.V;
    e0 = w0.e;
    P0 = w0.P;
    m  = D/V0;
    m2 = m*m;
    Vlast = VN.V;

    Detonation *det = HE.detonation(w0,w0.P);
    if( det==NULL )
        cerr << Error("ZNDprofile, HE.detonation failed" ) << Exit;
    if( det->u_s(D,RIGHT,CJ) )
        cerr << Error("ZNDprofile, det->u_s failed" ) << Exit;
    delete(det);

    N = HE.N();
    y  = new double[2+N];
    z  = y+2;
    yp = new double[2+N];
    zp = yp+2;
    y[0] = 0.0;     // M
    y[1] = 0.0;     // t
    z[0] = VN.P;    // Ps
    z[1] = 0.0;     // ts
    z[2] = 0.0;     // s1
    z[3] = 0.0;     // s2
    double dt;
    int status = HE.TimeStep(VN.V,VN.e,z,dt);
    if( status < 1 )
        cerr << Error("TimeStep failed") << Exit;
    //
    double dxi = VN.us*dt;
    double xi0 = 0.0;
    if( (status=Initialize(xi0, y, -dxi)) )
        cerr << Error("ZNDprofile, Initialize failed with status ")
             << ErrorStatus(status) << Exit;

    maxcount = 0;       // DEBUG
    sumcount = 0;       // DEBUG
}

void ZNDprofile::LocusState(double lambda1, double lambda2,
                            double &V, double &e, double &P, double &u)
{
    // state on strong branch of partly burned detonation locus
    // with mass flux m and burn fraction specified by preceeding
    // call to HE.set_eos(lambda1, lambda2)

    if( (Rate->Q==0.0 && lambda1==1.) || lambda2==1. )
    {   // at CJ state dfdV = 0
        V = CJ.V;
        e = CJ.e;
        P = CJ.P;
        u = CJ.u;
        return;
    }
    // 
    V = Vlast;              // initial guess for Newton iteration
    int count = iter_max;
    while( count-- )
    {
       double dV = V0-V;
       e = e0 + 0.5*m2*dV*dV + P0*dV;
       P = HE.HE->P(V,e);
       double h = e-e0 - 0.5*(P+P0)*(V0-V);
       if( debug )
       {
          cout << "Locus state: "
               << " V " << V
               << " e " << e
               << " P " << P
               << " h " << h
               << "\n";
       }
       if( std::abs(h) < iter_tol*(e-e0) ) break;

       double c2    = HE.HE->c2(V,e);
       double Gamma = HE.HE->Gamma(V,e);
       double f  = P-P0 - m2*(V0-V);
       double dfdV = Gamma*f/V + m2 - c2/(V*V);
       // at CJ state dfdV=0 since f=0 and m2=c2/(V*V)
       if( f<0. && dfdV>=0.0 )
       {    // exclude weak branch
            V = VN.V;
            continue;
       }
       // update
       V -= f/dfdV;
       V = min(V0, max(VN.V ,V) );
    }
    u = sqrt( (P-P0)*(V0-V) );
    Vlast = V;

    maxcount = max(maxcount, iter_max-count);     // DEBUG
    sumcount += iter_max-count;
}

int ZNDprofile::F(double *y_prime, const double *y_xi, double xi)
{
    const double *z_xi  = y_xi+2;
          double *zp = y_prime+2;

    double s1 = z_xi[2];
    double s2 = z_xi[3];
    double lambda1 = Rate->g(s1);
    double lambda2 = s2*s2;
    if( lambda2 >= 1. )
    {
        int i;
        for( i=0; i<N+2; i++ )
            y_prime[i] = 0.0;
        return 0;
    }

    HE.set_eos(lambda1, lambda2);
    double V,e,P,u;
    LocusState(lambda1, lambda2, V,e,P,u);
    double du = m*V;

    if( Rate->Rate(V,e,z_xi, zp) )
        return 1;

    y_prime[0] = -1./V;                     // M
    y_prime[1] = -1./du;                    // t
    y_prime[2] = 0.0;                       // Ps
    y_prime[3] = 0.0;                       // ts
    y_prime[4] = -zp[2]/du;                 // s1
    y_prime[5] = -zp[3]/du;                 // s2

    return 0;
}
int ZNDprofile::Xi(double &xi, double &V, double &e, double &P, double &u,
                   double &lambda1, double &lambda2)
{
    int status = Integrate(xi,y,yp);
    if( status )
        return status;
    double du = VN.us-y[1];
    V = du/m;

    // Correct shock state
    HE.get_lambda(z,lambda1, lambda2);
    if( lambda1 >= 1.-1.e-12 )
    {
        lambda1 = 1.0;
        HE.set_lambda(lambda1, lambda2, z);
    }
    HE.set_eos(lambda1, lambda2);
    LocusState(lambda1, lambda2, V,e,P,u);

    return 0;    
}

class lambda1_ODE : public ODEfunc
{
    HotSpotRateCC *Rate;
public:
    using ODEfunc::abs_tol;
    using ODEfunc::rel_tol;
    lambda1_ODE(HotSpotRateCC *rate) { Rate=rate;}
    double f(double V, const double *y, const double *yp);
};
double lambda1_ODE::f(double, const double *y, const double *yp)
{
    double s1 = y[4];
    return -Rate->g(s1);   // -lambda1
}
int ZNDprofile::Lambda1(double &lambda, double &xi,
                        double &V, double &e, double &P, double &u)
{
    lambda1_ODE L(Rate);
    L.abs_tol = abs_tol;
    L.rel_tol = rel_tol;
    lambda = -lambda;   // -lambda(xi)is monotonically increasing 
    int status = Integrate(L,lambda,xi,y,yp);
    lambda = -lambda;
    if( status )
        return status;
    double du = VN.us-y[1];
    V = du/m;

    // Correct shock state
    double lambda1, lambda2;
    HE.get_lambda(z,lambda1, lambda2);
    if( lambda1 >= 1.-1.e-12 )
    {
        lambda1 = 1.0;
        HE.set_lambda(lambda1, lambda2, z);
    }
    HE.set_eos(lambda1, lambda2);
    LocusState(lambda1, lambda2, V,e,P,u);

    return 0;    
}

class lambda2_ODE : public ODEfunc
{
public:
    using ODEfunc::abs_tol;
    using ODEfunc::rel_tol;
    lambda2_ODE() { }
    double f(double V, const double *y, const double *yp);
};
double lambda2_ODE::f(double, const double *y, const double *yp)
{
    double s2 = y[5];
    return -s2*s2;   // -lambda2
}
int ZNDprofile::Lambda2(double &lambda, double &xi,
                        double &V, double &e, double &P, double &u)
{
    lambda2_ODE L;
    L.abs_tol = abs_tol;
    L.rel_tol = rel_tol;
    lambda = -lambda;   // -lambda(xi)is monotonically increasing 
    int status = Integrate(L,lambda,xi,y,yp);
    lambda = -lambda;
    if( status )
        return status;
    double du = VN.us-y[1];
    V = du/m;

    // Correct shock state
    double lambda1, lambda2;
    HE.get_lambda(z,lambda1, lambda2);
    if( lambda2 >= 1.-1.e-12 )
    {
        lambda2 = 1.0;
        HE.set_lambda(lambda1, lambda2, z);
    }
    HE.set_eos(lambda1, lambda2);
    LocusState(lambda1, lambda2, V,e,P,u);

    return 0;    
}

void ZNDprofile::Last(double &xi, double &V, double &e, double &P, double &u)
{
    LastState(xi,y,yp);
    double du = VN.us-y[1];
    V = du/m;
    double lambda1, lambda2;
    HE.get_lambda(z,lambda1, lambda2);
    if( lambda2 >= 1.-1.e-12 )
    {
        lambda2 = 1.0;
        HE.set_lambda(lambda1, lambda2, z);
    }    
    HE.set_eos(lambda1, lambda2);
    LocusState(lambda1, lambda2, V,e,P,u);
}
const char *help[] = {    // list of commands printed out by help option
    "name        name    # material name",
    "material    name    # HEburn2::name",
    "file[s]     file    # colon separated list of data files",
    "lib         name    # directory for EOSlib shared libraries",
    "                    # default environ variable EOSLIB_SHARED_LIBRARY_PATH",
    "units       name    # default units from data base",
    "",
    "upiston     num     # particle velocity of detonation wave",
    "umin        num     # particle velocity at end of Taylor wave",
    "len         num     # length of Taylor wave",
    "dx          num     # delta x for profile",
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
//  ZNDprofile with Taylor wave
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

    double upiston =  0.;
    double umin    =  0.;       // velocity at end of Taylor Wave
    double len     = 10.;       // length of run for  Taylor Wave
    double dx      =  0.;       // output equally spaced in x

    double epsilon = 1.e-8;    // ODE tolerance
    double abs_tol = 1.e-14;   // ODEfunc tolerance
    double rel_tol = 1.e-8;    // ODEfunc tolerance


    int checkCJ = 0;
    int checkVN = 0;

    while(*++argv)
    {
        GetVar(file,files);
        GetVar(files,files);
	    GetVar(lib,lib);

        GetVar(name,name);
        //GetVar(type,type);
        GetVar(material,material);
        GetVar(units,units);
        GetVar(upiston,upiston);
        GetVar(umin,umin);
        GetVar(len,len);
        GetVar(dx,dx);
        GetVar(epsilon,epsilon);
        GetVar(abs_tol,abs_tol);
        GetVar(rel_tol,rel_tol);

        // debugging
        GetVarValue(checkCJ,checkCJ,1);
        GetVarValue(checkVN,checkVN,1);

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
        if( strcmp(type,"HEburn2") )
            cerr << Error("material type not HEburn2") << Exit;
	}
    if( name==NULL )
        cerr << Error("must specify (HEburn2::)name") << Exit;
    if( dx <= 0.0 )
        cerr << Error("must specify dx") << Exit;
    //
    DataBase db;
    if( db.Read(files) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    HEburn2 *HE = dynamic_cast<HEburn2 *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    HE->HE->tol_P = 1e-12;
    HE->HE->tol_T = 1e-4;
    HE->HE->lambda1_max = 0.99;
    HE->HE->lambda2_max = 0.99;
    HE->HE->max_iter  = 100;
    HE->HE->max_iter1 = 100;
    //cout << *(HE->HE);
    //return 1;
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
    EOS   *products = HE->Products();

    if( hug==NULL || det==NULL )
        cerr << Error("HE->detonation failed" ) << Exit;
    WaveState CJ;
    if( det->CJwave(RIGHT,CJ) )
        cerr << Error("det->CJwave failed" ) << Exit;
    //
    // check CJ state
    //
    if( checkCJ )
    {
        double Vcj = CJ.V;
        double ecj = CJ.e;
        double Pcj = products->P(Vcj,ecj);
        cerr << "CJ.V = " << Vcj << "\n";
        cerr << "CJ.P Pcj = " << CJ.P << " " << Pcj << "\n";
        double h = CJ.e - eref -0.5*(Pcj+Pref)*(Vref-Vcj);
        cerr << "ecj, h = " << ecj << " " << h << "\n";
        double Dcj = Vref*sqrt( (Pcj-Pref)/(Vref-Vcj) );
        double ucj = sqrt( (Pcj-Pref)*(Vref-Vcj) );
        double ccj = products->c(Vcj,ecj);
        double du = Dcj - (ucj+ccj);
        cerr << "Dcj ucj ccj = " << Dcj << " " << ucj << " " << ccj << "\n";
        cerr << "sonic: 1 -(ucj+ccj)/Dcj = " << " " << du/Dcj << "\n";

        return 1;
    }
    //
    upiston = max(upiston,CJ.u);
    WaveState OD;   // overdriven detonation if upiston > CJ.u
    if( det->u(upiston, RIGHT,OD) )
        cerr << Error("det->u failed" ) << Exit;
    double D = OD.us;   // detonation speed
    cout << "# ZNDprofile + Taylor wave for " << type << "::" << name << "\n";
    cout << "# D = " << D << "\n";
    //
    WaveState VN;
    if( hug->u_s(D,RIGHT,VN) )
        cerr << Error("hug->u_s failed") << Exit;
    if( checkVN )
    {
        double Vvn = VN.V;
        double evn = VN.e;
        EOS *reactants = HE->Reactants();
        double Pvn = reactants->P(Vvn,evn);
        cerr << "VN.P Pvn = " << VN.P << " " << Pvn << "\n";
        double h = VN.e - eref -0.5*(Pvn+Pref)*(Vref-Vvn);
        cerr << "evn, h = " << evn << " " << h << "\n";
        double Dvn = Vref*sqrt( (Pvn-Pref)/(Vref-Vvn) );
        cerr << "Dvn VN.us = " << Dvn << " " << VN.us << "\n";
        double uvn = sqrt( (Pvn-Pref)*(Vref-Vvn) );
        cerr << "uvn VN.u = " << uvn << " " << VN.u << "\n";

        deleteEOS(reactants);
        return 1;
    }

    delete det;
    delete hug;
    //
    // ZND profile
    //
    ZNDprofile profile(*HE,WS0,VN);
    profile.epsilon = epsilon;
    profile.abs_tol = abs_tol;
    profile.rel_tol = rel_tol;
    double *y = profile.y;
    double *z = profile.z;
    double *yp = profile.yp;
    double *zp = profile.zp;
    int N     = profile.N;
    double xi,t,M, lambda1,lambda2;
    double V, e,de, u, c, P, T, S;
    int i,j;
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
         << " " << setw(15) << "   lambda1";
    if( profile.Rate->Q > 0.0 )
    {
        cout << " " << setw(15) << "   lambda2"
             << " " << setw(15) << "   de";
    }
    cout << "\n";
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
         << " "  << setw(15) << " ";
    if( profile.Rate->Q > 0.0 )
    {
        cout << " " << setw(15) << "  "
             << " " << setw(15) << U->Unit("e");
    }
    cout << "\n";
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
    lambda1 = 0.0;
    lambda2 = 0.0;
    de      = 0.0;
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
             << " " << setw(15) << setprecision(8) << lambda1;
        if( profile.Rate->Q > 0.0 )
        {
             cout << " " << setw(15) << setprecision(8) << lambda2
                  << " " << setw(15) << setprecision(8) << de;
        }
        cout << "\n";
        t  = 0.0;
        xi = 0.0;
        M  = 0.0;
    }
    //
    // state at end of first reaction
    //
    lambda1 = 1. - 1.e-12;
    double xi_1;
    int status;
    if( (status=profile.Lambda1(lambda1,xi_1, V,e,P,u)) )
    {
        cerr << "ODE ERROR: status "
             << profile.ErrorStatus(status) << "\n";
        profile.Last(xi_1,V,e,P,u);
        HE->get_lambda(z,lambda1,lambda2);
        cerr << Error("profile.Lambda1 failed at lambda1 ")
             << lambda1 << " xi " << xi_1
             << Exit;
    }
    // profile for first reaction
    for( xi=0.; xi>xi_1; xi-=dx )
    {
        if( (status=profile.Xi(xi, V,e,P,u, lambda1,lambda2)) )
        {
            cerr << Error("profile.Xi failed with status")
                 << profile.ErrorStatus(status)
                 << Exit;
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
             << " " << setw(15) << setprecision(8) << lambda1;
        if( profile.Rate->Q > 0.0 )
        {
            lambda2 = z[3]*z[3];
            de = profile.Rate->q(lambda1,lambda2);
            cout << " " << setw(15) << setprecision(8) << lambda2
                 << " " << setw(15) << setprecision(8) << -de;
        }
        cout << "\n";
        if( status )
            cerr << Error("profile.Lambda1 failed with status ")
                 << profile.ErrorStatus(status) << Exit;
    }
    // last point of first reaction
    lambda1 = 1. - 1.e-12;
    status = profile.Lambda1(lambda1,xi_1, V,e,P,u);
    HE->get_lambda(z,lambda1,lambda2);
    M = -y[0];
    t =  y[1];

    P = HE->P(V,e,z);
    T = HE->T(V,e,z);
    c = HE->c(V,e,z);
    S = HE->S(V,e,z);
    cout        << setw(15) << setprecision(8) << t
         << " " << setw(15) << setprecision(8) << xi_1
         << " " << setw(15) << setprecision(8) << M
         << " " << setw(15) << setprecision(8) << V
         << " " << setw(15) << setprecision(8) << e
         << " " << setw(15) << setprecision(8) << u
         << " " << setw(15) << setprecision(8) << c
         << " " << setw(15) << setprecision(8) << P
         << " " << setw(15) << setprecision(8) << T
         << " " << setw(15) << setprecision(8) << S
         << " " << setw(15) << setprecision(8) << lambda1;
    if( profile.Rate->Q > 0.0 )
    {
        lambda2 = z[3]*z[3];
        de = profile.Rate->q(lambda1,lambda2);
        cout << " " << setw(15) << setprecision(8) << lambda2
             << " " << setw(15) << setprecision(8) << -de;
    }
    cout << "\n";
    lambda1 = 1.0;
    //
    // profile to end of second reaction
    //
    if( profile.Rate->Q > 0.0 )
    {
        if( (status=profile.Initialize(xi_1, y, -0.001*xi_1)) )
                cerr << Error("lambda2 profile.Initialize failed with status ")
                     << profile.ErrorStatus(status);

        lambda2 = 1. - 1.e-12;
        if( (status=profile.Lambda2(lambda2,xi_1, V,e,P,u)) )
        {
            cerr << "ODE ERROR: status "
                 << profile.ErrorStatus(status) << "\n";
            profile.Last(xi_1,V,e,P,u);
            HE->get_lambda(z,lambda1,lambda2);
            cerr << Error("profile.Lambda2 failed at lambda2 ")
                 << lambda2 << " xi " << xi_1
                 << Exit;
        }
        // profile to end of second reaction
        for( ; xi>xi_1; xi-=dx )
        {
            if( (status=profile.Xi(xi, V,e,P,u, lambda1,lambda2)) )
            {
                cerr << Error("profile.Xi failed with status")
                     << profile.ErrorStatus(status)
                     << Exit;
            }
            M = -y[0];
            t =  y[1];
            T = HE->T(V,e,z);
            c = HE->c(V,e,z);
            S = HE->S(V,e,z);
            de = profile.Rate->q(lambda1,lambda2);
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
                 << " " << setw(15) << setprecision(8) << lambda1
                 << " " << setw(15) << setprecision(8) << lambda2
                 << " " << setw(15) << setprecision(8) << -de
                 << "\n";
        }
        // last point of second reaction
        lambda2 = 1. - 0.99e-12;
        if( (status=profile.Lambda2(lambda2,xi_1, V,e,P,u)) )
        {
            cerr << "ODE ERROR: status "
                 << profile.ErrorStatus(status) << "\n";
            cerr << Error("profile.Lambda2 failed at lambda2 = 1")
                 << Exit;
        }
        
        M = -y[0];
        t =  y[1];
    
        P = HE->P(V,e,z);
        T = HE->T(V,e,z);
        c = HE->c(V,e,z);
        S = HE->S(V,e,z);
        de = profile.Rate->q(lambda1,lambda2);
        cout        << setw(15) << setprecision(8) << t
             << " " << setw(15) << setprecision(8) << xi_1
             << " " << setw(15) << setprecision(8) << M
             << " " << setw(15) << setprecision(8) << V
             << " " << setw(15) << setprecision(8) << e
             << " " << setw(15) << setprecision(8) << u
             << " " << setw(15) << setprecision(8) << c
             << " " << setw(15) << setprecision(8) << P
             << " " << setw(15) << setprecision(8) << T
             << " " << setw(15) << setprecision(8) << S
             << " " << setw(15) << setprecision(8) << lambda1
             << " " << setw(15) << setprecision(8) << lambda2
             << " " << setw(15) << setprecision(8) << -de
             << "\n";
        lambda2 = 1.0;
    }
    //
    // Taylor wave from detonation state
    // CJ or overdriven depending on whether upiston > ucj
    //
    if( len > 0. )
    {
      HydroState ZNDstate(V,e,u);
      Isentrope *I = products->isentrope(ZNDstate);
      if( I == NULL )
          cerr << Error("products->isentrope failed") << Exit;

      c = products->c(V,e);
      double ZND_xi  = xi_1;
      double ZND_M   = M;
      double ZND_upc = u + c;
      double ZND_rhoc = c/V;
      double t_TW  = len/D;
      t += t_TW;

      WaveState wave;
      u = umin;
      if( I->u(u,RIGHT,wave) )
          cerr << Error("I->u failed") << Exit;
      
      c = products->c(wave.V,wave.e);
      xi_1 = ZND_xi - (ZND_upc-(wave.u+c))*t_TW;
      for( ; xi>xi_1; xi-=dx )
      {
          double upc = ZND_upc + (xi-ZND_xi)/t_TW;
          if( I->u_s(upc,RIGHT,wave) )
          {
              cerr << Error("profile.Xi failed with status")
                   << profile.ErrorStatus(status)
                   << Exit;
          }
          T = products->T(wave.V,wave.e);
          c = products->c(wave.V,wave.e);
          S = products->S(wave.V,wave.e);
          M  = ZND_M  - (ZND_rhoc - c/wave.V)*t_TW;
          xi = ZND_xi - (ZND_upc-(wave.u+c))*t_TW;
          cout        << setw(15) << setprecision(8) << t
               << " " << setw(15) << setprecision(8) << xi
               << " " << setw(15) << setprecision(8) << M
               << " " << setw(15) << setprecision(8) << wave.V
               << " " << setw(15) << setprecision(8) << wave.e
               << " " << setw(15) << setprecision(8) << wave.u
               << " " << setw(15) << setprecision(8) << c
               << " " << setw(15) << setprecision(8) << wave.P
               << " " << setw(15) << setprecision(8) << T
               << " " << setw(15) << setprecision(8) << S
               << "\n";
      }
      // last point
      if( I->u(umin,RIGHT,wave) )
          cerr << Error("I->u failed") << Exit;
      T = products->T(wave.V,wave.e);
      c = products->c(wave.V,wave.e);
      S = products->S(wave.V,wave.e);
      M  = ZND_M  - (ZND_rhoc - c/wave.V)*t_TW;
      xi = ZND_xi - (ZND_upc-(wave.u+c))*t_TW;
      cout        << setw(15) << setprecision(8) << t
           << " " << setw(15) << setprecision(8) << xi
           << " " << setw(15) << setprecision(8) << M
           << " " << setw(15) << setprecision(8) << wave.V
           << " " << setw(15) << setprecision(8) << wave.e
           << " " << setw(15) << setprecision(8) << wave.u
           << " " << setw(15) << setprecision(8) << c
           << " " << setw(15) << setprecision(8) << wave.P
           << " " << setw(15) << setprecision(8) << T
           << " " << setw(15) << setprecision(8) << S
           << "\n";

      delete I;
    }
    deleteEOS(products);
    deleteEOS(eos);

    //cout << "DEBUG: maxcount " << profile.maxcount << "\n";
    //cout << "DEBUG: sumcount " << profile.sumcount << "\n";
    //cout << "DEBUG ODE steps " << profile.SizeFromLastOuterCachePoint() << "\n";
    //cout << "DEBUG ODE outer cache " << profile.OuterCacheSize() << "\n";

    return 0;
}
