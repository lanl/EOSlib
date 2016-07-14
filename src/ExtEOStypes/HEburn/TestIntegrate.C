#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include "HEburn.h"
#include "Arrhenius.h"
#include "PnRate.h"
#include "IgnitionGrowth.h"
#include "FFrate.h"
//
class IntegrateRate : public ODE
{
public:
    double abserr,relerr;
    HEburn &HE;
    double V, e;
    double debug;
    IntegrateRate(HEburn *he, double V0, double e0)
        : ODE(he->N(),512), HE(*he),V(V0), e(e0),
          abserr(1.e-8),relerr(1.e-8), debug(0) { }
    int Initialize(double t0, double *y);
    int F(double *y_prime, const double *y, double t); // ODE::F 
    double MaxNorm(const double *y0, const double *y1,
                   const double*, const double*);
    double dt(double lambda0, double lambda1);
};
int IntegrateRate::F(double *y_prime, const double *y, double t)
{
    if( y[0] >= 1.0 )
    {
        int i;
        for( i=0; i<Dimension(); i++ )
            y_prime[i] = 0.0;
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
double IntegrateRate::MaxNorm(const double *y0, const double *y1,
                   const double*, const double*)
{
	double Norm = 0;
	for(int i=Dimension(); i--; y0++, y1++)
    {
        double e1 = std::abs(*y0 - *y1)/((abs(*y0)+abs(*y1) + relerr)*relerr);
        double e2 = std::abs(*y0 - *y1)/abserr;
		Norm = max(Norm, min(e1,e2) );
    }		
	return Norm;
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
    const char *type = "HEburn";
    const char *name = "PBX9501.sam";
    const char *units = NULL;

    double P = 1.;
    double T = 1000.;
    double lambda = 0.0;
    double tmin = 0.0;
    double tmax = 0.0;
    int nsteps = 10;
    double abserr = 1.e-8;
    double relerr = 1.e-8;
    //
    int verbose = 1;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);

        GetVar(P,P);
        GetVar(T,T);
        GetVar(lambda,lambda);
        GetVar(tmin,tmin);
        GetVar(tmax,tmax);
        GetVar(nsteps,nsteps);
        GetVar(abserr,abserr);
        GetVar(relerr,relerr);
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
    HEburn *HE = dynamic_cast<HEburn *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    double lambda_ref = HE->lambda_ref();
    lambda = max(lambda,lambda_ref);
    int N = HE->N();
    double *Zref = &(*HE->Zref());
    int i;
    double y0[N], yp[N];
    for( i=0; i<N; i++ )
        y0[i] = Zref[i];
    y0[0] = lambda;
    HydroState PTstate;
    if( HE->PT(P,T,PTstate) )
        cerr << Error("PT failed") << Exit;
    double V = PTstate.V;
    double e = PTstate.e;
    P = HE->P(V,e,y0);
    T = HE->T(V,e,y0);
    cout << "Initial state: V, e = " << V << ", " << e
         << "  lambda = " << lambda << "\n";
    cout << "                 P, T = " << P << ", " << T << "\n";
    //
    int status = HE->Rate(V,e,y0,yp);
    if( status != N )
        cerr << Error("HE->Rate failed") << Exit;
    double tinduct = HE->Dt(V,e,y0,0.99);
    if( std::isnan(tinduct) )
        cerr << Error("Dt = NaN, check T(V,e) < T_cutoff") << Exit;        
    cout << "rate = " << yp[0]
         << ", time(lambda=0.99) =" << tinduct << "\n";
    double dt;
    status = HE->TimeStep(V,e,y0,dt);
    cout << "status = " << status << ", time step = " << dt << "\n";

    IntegrateRate Rate(HE,V,e);
    Rate.abserr = abserr;
    Rate.relerr = relerr;
    double t0 = 0.0;
    if( Rate.Initialize(t0,y0) )
        cerr << Error("Rate.Initialize failed with status ") << Exit;
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(10) << " t"
         << " " << setw(9) << "ODElambda"
         << " " << setw(9) << " HElambda"
         << "   " << setw(8) << " Rate"
         << " " << setw(5) << "  P"
         << " " << setw(5) << "  T"
         << "\n";
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::showpoint);
    if( tmax <= 0. )
        tmax = 1.1*tinduct;
    i = (tmin>0.) ? 0 : 1;
    for( ; i<=nsteps; i++ )
    {
        double t = tmin + double(i)/double(nsteps) * (tmax-tmin);
        double y[N];
        int ODEstatus = Rate.Integrate(t,y,yp);
        double lambda_t = y[0];
        int j;
        for( j=0; j<N; j++ )
            y[j] = y0[j];
        double dt = (i==0) ? tmin : (tmax-tmin)/double(nsteps);         
        if( HE->Integrate(V,e,y,dt) )
            cerr << Error("HE->Integrate failed") << Exit;
        for( j=0; j<N; j++ )
            y0[j] = y[j];          
        P = HE->P(V,e,y);
        T = HE->T(V,e,y);
        if( HE->Rate(V,e,y,yp) != N )
               cerr << Error("HE->Rate failed") << Exit;
        //
	    cout.setf(ios::scientific, ios::floatfield);
        cout << setw(10) << setprecision(4) << t;
	    cout.setf(ios::fixed, ios::floatfield);
        if( ODEstatus )
        {
           if( y[0] < 1. )
               cerr << Error("Rate.Integrate failed with status ")
                    << Rate.ErrorStatus(ODEstatus) << "\n";             
           cout << " " << setw(9) << "--- ";
        }
        else
           cout << " " << setw(9) << setprecision(6) << lambda_t;
        cout << " " << setw(9) << setprecision(6) << y[0];
	    cout.setf(ios::scientific, ios::floatfield);
        cout << "   " << setw(8) << setprecision(2) << yp[0];
	    cout.setf(ios::fixed, ios::floatfield);
        cout << " " << setw(5) << setprecision(1) << P
             << " " << setw(5) << setprecision(0) << T;
        if( verbose )
        { // diagnostic
            HErate  *rate = HE->rate;
            Arrhenius *AR = dynamic_cast<Arrhenius*>(rate);
            PnRate *Pn    = dynamic_cast<PnRate*>(rate);
            IgnitionGrowth *IG    = dynamic_cast<IgnitionGrowth*>(rate);
            FFrate *FF    = dynamic_cast<FFrate*>(rate);
            if( AR )
                cout << " " << setw(4) << -AR->min_level
                     << " " << setw(4) << AR->max_level
                     << " " << setw(4) << AR->count;
            else if( Pn )
                cout << " " << setw(4) << -Pn->min_level
                     << " " << setw(4) << Pn->max_level
                     << " " << setw(4) << Pn->count;
            else if( IG )
                cout << " " << setw(4) << -IG->min_level
                     << " " << setw(4) << IG->max_level
                     << " " << setw(4) << IG->count;
            else if( FF )
                cout << " " << setw(4) << -FF->min_level
                     << " " << setw(4) << FF->max_level
                     << " " << setw(4) << FF->count;
        }
        cout << "\n";
        if( ODEstatus && y[0] >= 1. )
            break;
        if( lambda_t >= 1. && y[0] >= 1. )
            break;
    }
    deleteEOS(eos);
    return 0;
}
