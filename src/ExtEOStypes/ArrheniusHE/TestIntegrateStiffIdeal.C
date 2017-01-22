#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include "StiffIdealHEav.h"
//
class ArrheniusRate : public ODE
{
public:
    StiffIdealHEav &HE;
    double V, e;
    ArrheniusRate(StiffIdealHEav *he, double V0,double e0,double *y);
    int F(double *y_prime, const double *y, double t); // ODE::F
    int Lambda(double lambda, double &t,double *y,double *yp);
};
ArrheniusRate::ArrheniusRate(StiffIdealHEav *he, double V0,double e0,double *y)
        : ODE(2,512), HE(*he),V(V0), e(e0)
{
    double dt = HE.tau;
    double t0 = 0.;
    int status = Initialize(t0, y, dt);
    if( status )
        cerr << Error("ArrheniusRate, Initialize failed with status ")
             << ErrorStatus(status) << Exit;   
}

int ArrheniusRate::F(double *y_prime, const double *y, double t)
{
    return (HE.Rate(V,e,y,y_prime)==2) ? 0 : 1;   
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
int ArrheniusRate::Lambda(double lambda, double &t,double *y,double *yp)
{
    lambda_ODE L;
    return Integrate(L,lambda,t,y,yp);
}

//
#define NaN EOS::NaN
int main(int, char **argv)
{
    EOS::Init();
    ProgName(*argv);
    //const char *file = "Test.data";
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ArrheniusTest.data";
    const char * file = file_.c_str();
    
    const char *type = "StiffIdealHEav";
    const char *name = "PBX9501";
    const char *units = NULL;

    double V = 0.9;//NaN;
    double e = 101;//NaN;
    double Tav = NaN;
    double lambda0 = 0.0;
    int nsteps = 10;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        //GetVar(type,type);
        GetVar(units,units);

        GetVar(V,V);
        GetVar(e,e);
        GetVar(Tav,Tav);
        GetVar(lambda,lambda0);
        GetVar(nsteps,nsteps);
        ArgError;
    }
    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    StiffIdealHEav *HE = dynamic_cast<StiffIdealHEav *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    double lambda_ref = HE->lambda_ref();
    if( std::isnan(V) || std::isnan(e) )
        cerr << Error("Must specify V & e") << Exit;
    lambda0 = max(lambda0,lambda_ref);    
    double t0 = 0.0;
    double y0[2] = {lambda0,0.0};
    double P = HE->P(V,e,y0);
    double T = HE->T(V,e,y0);
    y0[1] = std::isnan(Tav) ? T : Tav;
    cout << "Initial state: V, e = " << V       << ", " << e     << "\n"
         << "        lambda, Tav = " << lambda0 << ", " << y0[1] << "\n"
         << "               P, T = " << P       << ", " << T     << "\n";
    double t = HE->Dt(V,e,y0,0.9);
    cout << "Induction time (lambda>0.9) = " << t << "\n";
    ArrheniusRate Rate(HE,V,e,y0);
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(10) << " t"
         << " " << setw(8) << " dt"
         << " " << setw(6) << "lambda"
         << " " << setw(5) << " Tav"
         << " " << setw(8) << " Rate"
         << " " << setw(5) << "  P"
         << " " << setw(5) << "  T"
         << " " << setw(9) << " dlambda"
         << " " << setw(9) << "  dTav"
         << "\n";
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::showpoint);
    int i;
    for( i=1; i<=nsteps; i++ )
    {
        double lambda = lambda0 + (1.-lambda0)*double(i)/double(nsteps);
        double y[2],yp[2];
        double t;
        if( Rate.Lambda(lambda,t,y,yp) )
            cerr << Error("Rate.Lambda failed") << Exit;
        P = HE->P(V,e,y);
        T = HE->T(V,e,y);
        double dt = t - t0;
        if( HE->Integrate(V,e,y0,dt) )
            cerr << Error("HE.Integrate failed") << Exit;
        double dlambda = y0[0]-y[0];
        double dTav    = y0[1]-y[1];
	    cout.setf(ios::scientific, ios::floatfield);
        cout << setw(10) << setprecision(4) << t
             << " " << setw(8) << setprecision(2) << dt;
	    cout.setf(ios::fixed, ios::floatfield);
        cout << " " << setw(6) << setprecision(4) << y[0]   // lambda
             << " " << setw(5) << setprecision(0) << y[1];  // Tav
	    cout.setf(ios::scientific, ios::floatfield);
        cout << " " << setw(8) << setprecision(2) << yp[0];  // d(lambda)/dt
	    cout.setf(ios::fixed, ios::floatfield);
        cout << " " << setw(5) << setprecision(1) << P
             << " " << setw(5) << setprecision(0) << T;
	    cout.setf(ios::scientific, ios::floatfield);
        cout << " " << setw(9) << setprecision(2) << dlambda
             << " " << setw(9) << setprecision(2) << dTav
             << "\n";
        t0 = t;    
        y0[0] = y[0];
        y0[1] = y[1];
    }
    deleteEOS(eos);
    return 0;
}
