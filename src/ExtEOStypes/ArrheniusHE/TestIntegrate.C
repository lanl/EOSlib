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
    double debug;
    IntegrateRate(ArrheniusHE *he, double V0, double e0)
        : ODE(1,512), HE(*he),V(V0), e(e0), debug(0) { }
    int Initialize(double t0, double *y);
    int F(double *y_prime, const double *y, double t); // ODE::F 
    double dt(double lambda0, double lambda1);
};
int IntegrateRate::F(double *y_prime, const double *y, double t)
{
    if( y[0] >= 1.0 )
    {
        y_prime[0] = 0.0;
        return 0;
    }
    int status = HE.Rate(V,e,y,y_prime);
    if( debug )
    {
        cout << "status, t,y,yp = " << status << ", " << t << ", "
             << y[0] << ", " << y_prime[0] << "\n";
    }
    return (status==1) ? 0 : 1;   
}
double IntegrateRate::dt(double lambda0, double lambda1)
{
    double z[1];
    z[0] = lambda0;
    return HE.Dt(V,e,z,lambda1);
}
int IntegrateRate::Initialize(double t0, double *y)
{
    double lambda1 = min(y[0],0.999);
    double dt0 = HE.Dt(V,e,y,lambda1);
    int status = ODE::Initialize(t0, y, dt0);
    if( status )
        cerr << Error("Initialize failed with status ")
             << ErrorStatus(status);
    return status;  
}


//
#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "Test.data";
    const char *type = "ArrheniusHE";
    const char *name = "HMX";
    const char *units = NULL;

    double V = NaN;
    double e = NaN;
    double t = 1.0;
    double lambda = 0.0;

    EOS::Init();
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);

        GetVar(V,V);
        GetVar(e,e);
        GetVar(t,t);
        GetVar(lambda,lambda);
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
    double lambda_ref = HE->lambda_ref();
    if( std::isnan(V) || std::isnan(e) )
        cerr << Error("Must specify V & e") << Exit;
    lambda = max(lambda,lambda_ref);    
    double y[1] = {lambda};
    double yp[1];
    double P = HE->P(V,e,y);
    double T = HE->T(V,e,y);
    cout << "Initial state: V, e = " << V << ", " << e
         << "  lambda = " << lambda << "\n";
    cout << "                 P, T = " << P << ", " << T << "\n";
    //
    int status = HE->Rate(V,e,y,yp);
    if( status != 1 )
        cerr << Error("HE->Rate failed") << Exit;
    double tinduct = HE->Dt(V,e,y,0.99);
    if( std::isnan(tinduct) )
        cerr << Error("Dt = NaN, check T(V,e) < T_cutoff") << Exit;        
    cout << "rate = " << yp[0]
         << ", time(lambda=0.99) =" << tinduct << "\n";
    t = min(t,10.*tinduct);
    double dt;
    status = HE->TimeStep(V,e,y,dt);
    cout << "status = " << status << ", time step = " << dt << "\n";

    IntegrateRate Rate(HE,V,e);
    double t0 = 0.0;
    if( Rate.Initialize(t0,y) )
        cerr << Error("Rate.Initialize failed with status ") << Exit;
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(10) << " t"
         << " " << setw(6) << "lambda"
         << " " << setw(8) << " Rate"
         << " " << setw(5) << "  P"
         << " " << setw(5) << "  T"
         << "\n";
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::showpoint);
        y[0] = lambda;
        /*************
        double td = 0.0026;
        status = Rate.Integrate(td,y,yp);
        Rate.debug = 1;
        *************/
        status = Rate.Integrate(t,y,yp);
        if ( status )
            cerr << Error("Rate.Integrate failed with status ")
                 << Rate.ErrorStatus(status) << "\n";
        P = HE->P(V,e,y);
        T = HE->T(V,e,y);
	    cout.setf(ios::scientific, ios::floatfield);
        cout << setw(10) << setprecision(4) << t;
	    cout.setf(ios::fixed, ios::floatfield);
        if( status )
        {
           cout << " " << setw(6) << "---";
           cout << " " << setw(8) << "---";            
        }
        else
        {
           cout << " " << setw(6) << setprecision(4) << y[0];
	       cout.setf(ios::scientific, ios::floatfield);
           cout << " " << setw(8) << setprecision(2) << yp[0];
        }
	    cout.setf(ios::fixed, ios::floatfield);
        cout << " " << setw(5) << setprecision(1) << P
             << " " << setw(5) << setprecision(0) << T
             << "\n";
        y[0] = lambda;
        status = HE->Integrate(V,e,y,t);
        cout << "status=" << status
             << ", lambda = " << setprecision(4) << y[0] << "\n";
    deleteEOS(eos);
    return 0;
}
