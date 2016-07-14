#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include <HEburn.h>
#include <EOS_VT.h>
//
// Calculates time evolution at constant volume for Adiabatic burn
//
class IntegrateRate : public ODE
{
public:
    double abserr,relerr;
    HEburn &HE;
    int N;
    double *Zref;
    double V, e;
    double debug;
    IntegrateRate(HEburn *he)
        : ODE(he->N(),512), HE(*he), abserr(1.e-8),relerr(1.e-8), debug(0)
        { N=HE.N(); Zref = &(*HE.Zref()); }
    int Initialize(double t0, double V0, double e0, double *y);
    int F(double *y_prime, const double *y, double t); // ODE::F 
    double MaxNorm(const double *y0, const double *y1,
                   const double*, const double*);
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

int IntegrateRate::Initialize(double t0, double V0, double e0, double *y)
{
    V = V0;
    e = e0;
    int i;
    for( i=0; i<N; i++ )
        y[i] = Zref[i];
    double dt;
    if( HE.TimeStep(V,e, y, dt) < 1 )
    {
        cerr << Error("Initialize::TimeStep failed\n");
        return 1;
    }  
    int status = ODE::Initialize(t0, y, dt);
    if( status )
        cerr << Error("Initialize failed with status ")
             << ErrorStatus(status) << "\n";             
    return status;  
}
class lambda_ODE : public ODEfunc
{
public:
    lambda_ODE() {}
    double f(double t, const double *y, const double *yp);
};
double lambda_ODE::f(double, const double *y, const double *yp)
{
    return y[0];   // lambda
}


//
#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file     = NULL;
    const char *type     = "HEburn";
    const char *name     = NULL;
	const char *material = NULL;
    const char *units    = NULL;
    const char *lib      = NULL;
    //
    double T0 = 1000.;
    double lambda_ref = 0.5;
    double lambda1    = 0.9;
    int nsteps = 10;        // number of lambda steps
    int tsteps = 10;        // number of t steps
    double abserr = 1.e-8;
    double relerr = 1.e-8;
    //
    int verbose = 1;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
	    GetVar(lib,lib);
        GetVar(name,name);
        //GetVar(type,type);HEburnRate.C
        GetVar(material,material);
        GetVar(units,units);

        GetVar(T0,T0);
        GetVar(lambda1,lambda1);
        GetVar(lambda_ref,lambda_ref);
        GetVar(tsteps,tsteps);
        GetVar(nsteps,nsteps);
        GetVar(abserr,abserr);
        GetVar(relerr,relerr);
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
    EOS *Reactants = HE->Reactants();
    if( Reactants == NULL )
        cerr << Error("Reactants is NULL") << Exit;
    EOS_VT *VTreactants = Reactants->eosVT();
    if( VTreactants == NULL )
        cerr << Error("VTreactants is NULL") << Exit;
    const int N = HE->N();
    double V0 = eos->V_ref;
    double e0 = VTreactants->e(V0,T0);
    deleteEOS(Reactants);
    deleteEOS_VT(VTreactants);
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(10) << "  t"
         << " " << setw(10) << " t/t_ref"
         << " " << setw(10) << " t/t_ref-1"
         << " " << setw(10) << " lambda"
         << " " << setw(10) << " 1-lambda"
         << " " << setw(10) << " P"
         << " " << setw(10) << " T"
         << " " << setw(10) << " Rate"
         << "\n";
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);
    //
    IntegrateRate Rate(HE);
    lambda_ODE Lambda;
    Rate.abserr = abserr;
    Rate.relerr = relerr;
    double y[N],yp[N];
    double t = 0.0;
    if( Rate.Initialize(t,V0,e0,y) )
        cerr << Error("Rate.Initialize failed") << Exit;
    double t_ref;
    int ODEstatus = Rate.Integrate(Lambda,lambda_ref,t_ref,y,yp);
    if( ODEstatus )
        cerr << Error("Rate.Integrate failed with status ")
             << Rate.ErrorStatus(ODEstatus) << "\n";
    double lambda = lambda1/double(nsteps);
    double lambda_t;
    int i;
    for( i=0; i<=tsteps; i++ )
    {
        t = double(i)/double(tsteps)*t_ref;
        int ODEstatus = Rate.Integrate(t,y,yp);
        if( ODEstatus )
            cerr << Error("Rate.Integrate failed with status ")
                 << Rate.ErrorStatus(ODEstatus) << "\n";
        if( y[0] > lambda )
           break;
        lambda_t = y[0];
        double tp = t/t_ref;
        double T = HE->T(V0,e0,y);
        double P = HE->P(V0,e0,y);
        cout <<        setw(10) << setprecision(4) << t
             << " " << setw(10) << setprecision(4) << tp
             << " " << setw(10) << setprecision(4) << tp-1.
             << " " << setw(10) << setprecision(4) << y[0]
             << " " << setw(10) << setprecision(4) << 1.-y[0]
             << " " << setw(10) << setprecision(4) << P
             << " " << setw(10) << setprecision(4) << T
             << " " << setw(10) << setprecision(4) << yp[0]
             <<  "\n";
    }
    for( i=0; i<=nsteps; i++ )
    {
        lambda = double(i)/double(nsteps)*lambda1;
        if( lambda <= lambda_t )
            continue;
        int ODEstatus = Rate.Integrate(Lambda,lambda,t,y,yp);
        if( ODEstatus )
            cerr << Error("Rate.Integrate failed with status ")
                 << Rate.ErrorStatus(ODEstatus) << "\n";
        double tp = t/t_ref;
        double T = HE->T(V0,e0,y);
        double P = HE->P(V0,e0,y);
        cout <<        setw(10) << setprecision(4) << t
             << " " << setw(10) << setprecision(4) << tp
             << " " << setw(10) << setprecision(4) << tp-1.
             << " " << setw(10) << setprecision(4) << lambda
             << " " << setw(10) << setprecision(4) << 1.-lambda
             << " " << setw(10) << setprecision(4) << P
             << " " << setw(10) << setprecision(4) << T
             << " " << setw(10) << setprecision(4) << yp[0]
             <<  "\n";
    }
    deleteEOS(eos);
    return 0;
}
