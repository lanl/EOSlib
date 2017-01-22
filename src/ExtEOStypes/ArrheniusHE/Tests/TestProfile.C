#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include "ArrheniusHE.h"
//
class IntegrateRate : public ODE
{
public:
    ArrheniusHE &HE;
    double V, e;
    double abs_err, rel_err;
    IntegrateRate(ArrheniusHE *he, double V0, double e0)
        : ODE(1,512), HE(*he),V(V0), e(e0)
        { abs_err = 1e-6; rel_err = 1e-3;}
    int F(double *y_prime, const double *y, double t); // ODE::F 
    int Lambda(double &lambda, double &t, double &rate);
};
int IntegrateRate::F(double *y_prime, const double *y, double t)
{
    if( y[0] >= 1.0 )
    {
        y_prime[0] = 0.0;
        return 0;
    }
    return (HE.Rate(V,e,y,y_prime)!=1) ? 1 : 0;   
}
class lambda_ODE : public ODEfunc
{
public:
    lambda_ODE() {}
    double f(double V, const double *y, const double *yp);
};
double lambda_ODE::f(double, const double *y, const double *yp)
{
    return y[0];
}
int IntegrateRate::Lambda(double &lambda, double &t, double &rate)
{
    lambda_ODE L;
    double y[1];
    double yp[1];
    int status = Integrate(L,lambda,t,y,yp);
    if( status )
        return status;
    lambda = y[0];
    rate   = yp[0];
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
    double Ps = 20.;
    int nsteps = 20;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        GetVar(Ps,Ps);
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
    cout << "Reference state: V, e = " << Vref << ", " << eref
         << "  lambda = " << lambda_ref << "\n";
    cout << "                 P, T = " << Pref << ", " << Tref << "\n";
    //
    HydroState state0(Vref,eref);
    Hugoniot *shock = HE->shock(state0);
    if( shock == NULL )
        cerr << Error("HE->shock failed" ) << Exit;
    WaveState Pstate;
    if( shock->P(Ps,RIGHT,Pstate) )
        cerr << Error("shcok->P failed" ) << Exit;
    double t1 = HE->Dt(Pstate.V,Pstate.e,HE->z_f(Pstate.V,Pstate.e),0.9);
    cout << "t1=" << t1 << "\n";

    IntegrateRate Rate(HE,Pstate.V,Pstate.e);
    double t = 0;
    double dt_step = 0.01*t1;
    double y[1] = {lambda_ref};
    int status = Rate.Initialize(t, y, dt_step);
    if( status )
        cerr << Error("Rate.Initialize failed with status ")
             << Rate.ErrorStatus(status) << Exit;
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(10) << " t"
         << " " << setw(6) << "lambda"
         << " " << setw(8) << " Rate"
         << " " << setw(5) << "  P"
         << " " << setw(5) << "  T"
         << " " << setw(8) << " dt"
         << "\n";
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::showpoint);
    int i;
    for( i=0; i<=nsteps; i++ )
    {
        double lambda = double(i)/double(nsteps);
        if( lambda < lambda_ref )
            continue;
        double t, rate;
    // integrate to specified values of lambda
        status = Rate.Lambda(lambda,t,rate);
        if ( status )
            cerr << Error("Rate.Integrate failed with status ")
                 << Rate.ErrorStatus(status) << Exit;
        y[0] = lambda;
        double P = HE->P(Pstate.V,Pstate.e,y);
        double T = HE->T(Pstate.V,Pstate.e,y);
        double dt;
        if( HE->TimeStep(Pstate.V,Pstate.e,y,dt) == -1 )
            cerr << Error("TimeStep failed") << Exit;            
	    cout.setf(ios::scientific, ios::floatfield);
        cout << setw(10) << setprecision(4) << t;
	    cout.setf(ios::fixed, ios::floatfield);
        cout << " " << setw(6) << setprecision(4) << lambda;
	    cout.setf(ios::scientific, ios::floatfield);
        cout << " " << setw(8) << setprecision(2) << rate;
	    cout.setf(ios::fixed, ios::floatfield);
        cout << " " << setw(5) << setprecision(1) << P
             << " " << setw(5) << setprecision(0) << T;
	    cout.setf(ios::scientific, ios::floatfield);
        cout <<  " " << setw(8) << setprecision(2) << dt        
             << "\n";
    }
    deleteEOS(products);
    deleteEOS(reactants);
    deleteEOS(eos);
    return 0;
}
