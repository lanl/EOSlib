#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include "ArrheniusHE.h"
//
class SteadyProfile : public ODE
{
// xi = x - D*t <= 0, right facing wave
// (D-u) = (d/dxi) lambda  = -Rate
// [c^2-(D-u)^2] (d/dxi) u = V(\partialP/\partial\lambda)*Rate
public:
    ArrheniusHE &HE;
    WaveState ws;
    double m;       // mass flux
    double B;       // Bernoulli constant = e + PV + 0.5*(D-u)^2
    double t1;      // initial induction time
    // D = W0.us
    double abs_err, rel_err;
    SteadyProfile(ArrheniusHE *he, WaveState w);
    int F(double *y_prime, const double *y, double t); // ODE::F 
    int Lambda(double &lambda, double &xi, double *y,
               double &V, double &e, double &P);
};
SteadyProfile::SteadyProfile(ArrheniusHE *he, WaveState w)
        : ODE(2,512), HE(*he), ws(w)
{
    abs_err = 1e-6;
    rel_err = 1e-6;
    double du = ws.us-ws.u;
    m = du/ws.V;
    B = ws.e + ws.P*ws.V + 0.5*du*du;
    double y[2];
    y[0] = HE.lambda_ref();
    y[1] = ws.u;
    t1 = HE.Dt(ws.V,ws.e,y,0.9);
    double dxi = 0.01*ws.us*t1;
    double xi0 = 0.0;
    int status = Initialize(xi0, y, -dxi);
    if( status )
        cerr << Error("SteadyProfile, Initialize failed with status ")
             << ErrorStatus(status) << Exit;
        
}
int SteadyProfile::F(double *y_prime, const double *y, double t)
{
    double lambda = y[0];
    double u      = y[1];
    double du = ws.us-u;
    if( lambda > 1. || du < 0. )
        return 1;
    double V  = du/m;
    double P  = ws.P +  m*(u-ws.u);
    double e = B - (P*V+0.5*du*du);
    if( HE.Rate(V,e,y, y_prime)!=1 )
        return 1;
    double rate = y_prime[0];
    double c2 = HE.c2(V,e,y);
    if( c2 <= du*du )
        return 1;
    double dP, dT;
    if( HE.ddlambda(V,e,lambda,dP,dT) )
        return 1;
    y_prime[0] = -rate/du;
    y_prime[1] = V*dP*rate/(c2-du*du);
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
    return -y[0];
}
int SteadyProfile::Lambda(double &lambda, double &xi, double *y,
                          double &V, double &e, double &P)
{
    lambda_ODE L;
    double yp[2];
    int status = Integrate(L,lambda,xi,y,yp);
    if( status )
        return status;
    double u = y[1];
    double du = ws.us-u;
    V = du/m;
    P  = ws.P +  m*(u-ws.u);
    e = B - (P*V+0.5*du*du);
    return 0;    
}
//
#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    //const char *file = "Test.data";
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ArrheniusTest.data";
    const char * file = file_.c_str();
    
    const char *type = "ArrheniusHE";
    const char *name = "HMX";
    const char *units = NULL;

    EOS::Init();
    int nsteps = 20;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        GetVar(nsteps,nsteps);
        ArgError;
    }
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    ArrheniusHE *HE = dynamic_cast<ArrheniusHE *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    EOS *reactants = HE->reactants();
    EOS *products  = HE->products();
    double lambda_ref = HE->lambda_ref();
    //
    double Vref = eos->V_ref; 
    double eref = eos->e_ref;
    double Pref = HE->P(Vref,eref);
    double Tref = HE->T(Vref,eref);
    WaveState CJ;
    HydroState state0(Vref,eref);
    Detonation *det = HE->detonation(state0,Pref);
    if( det == NULL )
        cerr << Error("HE->detonation failed" ) << Exit;
    if( det->CJwave(RIGHT,CJ) )
        cerr << Error("det->CJwave failed" ) << Exit;
    double T = products->T(CJ.V,CJ.e);
    cout << "CJ state\n";
    cout << "V e P T = " << CJ.V << ", " << CJ.e 
                 << ", " << CJ.P << ", " << T << "\n";
    cout << "D, u, c = " << CJ.us << ", " << CJ.u
                 << ", " << products->c(CJ.V,CJ.e) << "\n";
    double Dcj = CJ.us;
    double Pcj = CJ.P;
    Hugoniot *hug = reactants->shock(state0);
    if( hug == NULL )
        cerr << Error("eos1->shock failed") << Exit;
    WaveState PCJ;
    if( hug->P(Pcj,RIGHT,PCJ) )
        cerr << Error("hug->P failed") << Exit;
    T = reactants->T(PCJ.V,PCJ.e);
    double z[1] = {lambda_ref};
    double t1 = HE->Dt(PCJ.V,PCJ.e,z,0.9);
    cout << "reactant shock to Pcj: T = " << T
         << ", induction time = " << t1 << "\n";

    
    WaveState VN;
    if( hug->u_s(Dcj,RIGHT,VN) )
        cerr << Error("hug->u_s failed") << Exit;
    T = reactants->T(VN.V,VN.e);
    cout << "VN spike state\n";
    cout << "V e u = " << VN.V << ", " << VN.e << ", " << VN.u << "\n";
    cout << "  P T = " << VN.P << ", " << T << "\n";
    

    SteadyProfile profile(HE,VN);
    cout << "induction time " << profile.t1 << " (estimate)\n";
    cout << "xi = us*T_induction = " << profile.t1*CJ.us << "\n";
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(9) << "# xi"
         << " " << setw(8) << " time"
         << " " << setw(6) << "lambda"
         << " " << setw(6) << "  u"
         << " " << setw(6) << "  V"
         << " " << setw(6) << "  e"
         << " " << setw(5) << "  P"
         << " " << setw(5) << "  T"
         << " " << setw(6) << " u+c-D"
         << " " << setw(8) << " tinduct"
         << " " << setw(8) << " dt"
         << "\n";
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::showpoint);
    int i;
    for( i=0; i<=nsteps+4; i++ )
    {
        double lambda;
        if( i < nsteps )
        {
            lambda= double(i)/double(nsteps);
            if( lambda < lambda_ref )
                continue;
        }
        else if( i == nsteps )
            lambda = 0.99;
        else
            lambda = 1. - 0.1*(1.-lambda);
        double V, e, P;
        double xi;
        double y[2];
        // integrate to specified values of lambda
        lambda = -lambda;
        int status = profile.Lambda(lambda,xi,y, V,e,P);
        if ( status )
            cerr << Error("Rate.Integrate failed with status ")
                 << profile.ErrorStatus(status) << Exit;
        lambda   = y[0];
        double u = y[1];
        //double P = HE->P(Pstate.V,Pstate.e,y);
        double T = HE->T(V,e,y);
        double t = xi/CJ.us;
        if( t < 0 )
            t = -t;
        double c2 = HE->c2(V,e,y);
        double c = (c2>0.) ? sqrt(c2) : 0.0;
        double ucD = u+c-CJ.us;
        t1 = HE->Dt(V,e,y,0.99);
        double dt;
        if( HE->TimeStep(V,e,y,dt) == -1 )
            cerr << Error("TimeStep failed") << Exit;            
	    cout.setf(ios::scientific, ios::floatfield);
        cout <<        setw(9) << setprecision(2) << xi
             << " " << setw(8) << setprecision(2) << t;
	    cout.setf(ios::fixed, ios::floatfield);
        cout << " " << setw(6) << setprecision(4) << lambda
             << " " << setw(6) << setprecision(4) << u
             << " " << setw(6) << setprecision(4) << V
             << " " << setw(6) << setprecision(4) << e
             << " " << setw(5) << setprecision(2) << P
             << " " << setw(5) << setprecision(0) << T
             << " " << setw(6) << setprecision(4) << ucD;
	    cout.setf(ios::scientific, ios::floatfield);
        cout << " " << setw(8) << setprecision(2) << t1        
             << " " << setw(8) << setprecision(2) << dt        
             << "\n";
    }   
    deleteEOS(products);
    deleteEOS(reactants);
    deleteEOS(eos);
    return 0;
}
