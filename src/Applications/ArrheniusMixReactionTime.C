#include <Arg.h>
#include <ODE.h>
#include "HEburn.h"
#include "Arrhenius.h"
#include "EOS_VT.h"
//
// Calculates Adiabatic induction time at constant volume
// Reaction from 0 to lambda1 for HEburn type material
// with modified mixture EOS:
//   Fmix(V,T) = lambda*F_products(V,T) + (1-lambda)*F_reactants(V,T)
//   Pmix(V,T) = lambda*P_products(V,T) + (1-lambda)*P_reactants(V,T)
//   Smix(V,T) = lambda*S_products(V,T) + (1-lambda)*S_reactants(V,T)
//   emix(V,T) = lambda*e_products(V,T) + (1-lambda)*e_reactants(V,T)
//
class IntegrateRate : public ODE
{
public:
    double abserr,relerr;
    HEburn &HE;
    EOS *Reactants;
    EOS *Products;
    Arrhenius *Arate;
    EOS_VT *VTreactants;
    EOS_VT *VTproducts;
    double V, e;
    //
    IntegrateRate(HEburn *he);
    ~IntegrateRate();
    int Initialize(double t0, double V0, double *y);
    // y[0] = lambda
    // y[1] = T
    int F(double *y_prime, const double *y, double t); // ODE::F
    // dlambda/dt = Rate(lambda,T)
    //      dT/dt = [(e_r-e_p)/(lambda*CV_p+(1-lambda)*CV_r)]*dlambda/dt
    double MaxNorm(const double *y0, const double *y1,
                   const double*, const double*);
    double P(double lambda, double T)
        { return lambda*VTproducts->P(V,T) +(1.-lambda)*VTreactants->P(V,T);}
    double emix(double lambda, double T)
        { return lambda*VTproducts->e(V,T) +(1.-lambda)*VTreactants->e(V,T);}
    double CV(double lambda, double T)
        { return lambda*VTproducts->CV(V,T)+(1.-lambda)*VTreactants->CV(V,T);}
    double rate(double lambda, double T)
        { return Arate->Rate(lambda,T);}
};
IntegrateRate::IntegrateRate(HEburn *he)
        : ODE(2,512), HE(*he), abserr(1.e-8),relerr(1.e-8)
{
    Reactants = HE.Reactants();
    if( Reactants == NULL )
        cerr << Error("Reactants is NULL") << Exit;
    Products = HE.Products();
    if( Products == NULL )
        cerr << Error("Products is NULL") << Exit;
    VTreactants = Reactants->eosVT();
    if( VTreactants == NULL )
        cerr << Error("VTreactants is NULL") << Exit;
    VTproducts = Products->eosVT();
    if( VTproducts == NULL )
        cerr << Error("VTproducts is NULL") << Exit;
    Arate = dynamic_cast<Arrhenius *>(HE.rate);
    if( Arate == NULL )
        cerr << Error("Arate is NULL") << Exit;
}
IntegrateRate::~IntegrateRate()
{
    deleteEOS_VT(VTproducts);
    deleteEOS_VT(VTreactants);
    deleteEOS(Products);
    deleteEOS(Reactants);
}

int IntegrateRate::F(double *y_prime, const double *y, double t)
{
    if( y[0] >= 1.0 )
    {
        y_prime[0] = 0.0;
        y_prime[1] = 0.0;
        return 0;
    }
    double lambda = y[0];
    double T      = y[1];
    double e_r = VTreactants->e(V,T);
    double e_p = VTproducts->e(V,T);
    y_prime[0] = rate(lambda,T);
    y_prime[1] = (e_r-e_p)/CV(lambda,T)*y_prime[0];    
    return (std::isnan(y_prime[0])) ? 1 : 0;   
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

int IntegrateRate::Initialize(double t0, double V0, double *y)
{
    V = V0;
    double T = y[1];
    double lambda = y[0];
    e = emix(lambda,T);
    double dt = 0.01/rate(lambda,T);
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
//
#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file     = NULL;
    const char *type     = strdup("HEburn");
    const char *name     = NULL;
	const char *material = NULL;
    const char *units    = NULL;
    const char *lib      = NULL;
    //
    double T1 = 1000.;
    double T2 = 3000.;
    double lambda1 = 0.5;
    int nsteps = 10;
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
        //GetVar(type,type);
        GetVar(material,material);
        GetVar(units,units);

        GetVar(T1,T1);
        GetVar(T2,T2);
        GetVar(lambda1,lambda1);
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
    double V0 = eos->V_ref;
    HEburn *HE = dynamic_cast<HEburn *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    double lambda0 = HE->lambda_ref();
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(10) << "  T1"
         << " " << setw(10) << " P1"
         << " " << setw(10) << " Cv"
         << " " << setw(10) << " 1/Rate"
         << " " << setw(10) << " t@lambda1"
         << " " << setw(10) << " T2"
         << " " << setw(10) << " P2"
         << " " << setw(10) << " t*Rate"
         << " " << setw(10) << " 1/(Ta*Q)"
         << "\n";
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);
    //
    IntegrateRate Rate(HE);
    lambda_ODE Lambda;
    Rate.abserr = abserr;
    Rate.relerr = relerr;
    int i;
    for( i=0; i<=nsteps; i++ )
    {
        double T =(i*T2 +(nsteps-i)*T1)/nsteps;
        double y[2] = {lambda0,T};
        double dt;
        double t = 0.0;
        if( Rate.Initialize(t,V0,y) )
            cerr << Error("Rate.Initialize failed with status ") << Exit;
        double P1 = Rate.P(lambda0,T);
        double Cv = Rate.CV(lambda0,T);
        double rate = Rate.rate(lambda0,T);
        int ODEstatus = Rate.Integrate(Lambda,lambda1,t,y,NULL);
        if( ODEstatus )
            cerr << Error("Rate.Integrate failed with status ")
                 << Rate.ErrorStatus(ODEstatus) << "\n";             
        double T2 = y[1];
        double P2 = Rate.P(lambda1,T2);
        double ratio = t*rate/(T*T*Cv);   // 1/(Ta*Q)
        //double emix2 = Rate.emix(lambda1,T2);
        //double emix1 = Rate.emix(lambda0,T);
        cout <<        setw(10) << setprecision(4) << T
             << " " << setw(10) << setprecision(4) << P1
             << " " << setw(10) << setprecision(4) << Cv
             << " " << setw(10) << setprecision(4) << 1./rate
             << " " << setw(10) << setprecision(4) << t
             << " " << setw(10) << setprecision(4) << T2
             << " " << setw(10) << setprecision(4) << P2
             << " " << setw(10) << setprecision(4) << t*rate
             << " " << setw(10) << setprecision(4) << ratio
             //<< " " << setw(10) << setprecision(4) << emix1
             //<< " " << setw(10) << setprecision(4) << emix2
             <<  "\n";
    }
    deleteEOS(eos);
    return 0;
}
