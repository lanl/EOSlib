#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include <EOS.h>
#include <EOS_VT.h>
//
// Calculates adiabatic induction time at constant volume
// for three-step McGuire-Tarver burn model
//
class ThreeStep
{
public:
    double lnZ1,Ta1,Q1;
    double lnZ2,Ta2,Q2;
    double lnZ3,Ta3,Q3;
    double Rate1(double T) { return exp(lnZ1-Ta1/T);}
    double Rate2(double T) { return exp(lnZ2-Ta2/T);}
    double Rate3(double T) { return exp(lnZ3-Ta3/T);}
    ThreeStep();
    ~ThreeStep() {}
};

ThreeStep::ThreeStep()
{
// McGuire-Tarver model parameters for LX-10 (94.5% HMX + 5.5% Viton)
// Critical Conditions for Impact- and Shock-Induced Hot Spots in Solid Explosives
// Tarver, Chidester and Nichols
// J. Phys. Chem. 100 (1996) pp. 5794-5799 (table 1.2)
    double R =  1.987e-3;           // kcal/mole/K
    double loge6 = log(1.e6);       // seconds to microseconds
    // T in degrees K
    // Rate in inverse microseconds
    Ta1  = 52.7/R;
    lnZ1 = 48.7-loge6;
    Ta2  = 44.1/R;
    lnZ2 = 37.3-loge6;
    Ta3  = 34.1/R;
    lnZ3 = 28.1-loge6;
    double energy = 4.1868e-3;      // cal/g -> kJ/g
    // heat of reaction
    Q1 = -100. * energy;            // endothermic
    Q2 =  300. * energy;            // exothermic
    Q3 = 1200. * energy;            // exothermic
    // total Q = 5.86 MJ/kg
}
//
//
//
class IntegrateRate : public ODE
{
// y[4] = {T, lambda1, lambda2, lambda3}
public:
    enum integrate {adiabatic,isothermal};
    double abserr,relerr;
    ThreeStep &TS;
    EOS_VT *VT; 
    double V0;
    double debug;
    integrate evolution;
    IntegrateRate(integrate e, ThreeStep &ts, EOS_VT *eos)
        : ODE(4,512), evolution(e), TS(ts),VT(eos),
          abserr(1.e-8),relerr(1.e-8), debug(0) { }
    int Initialize(double t0, double V0, double *y);
    int F(double *y_prime, const double *y, double t); // ODE::F 
};
int IntegrateRate::F(double *y_prime, const double *y, double t)
{
// lambda is mass fraction
// lambda0 ----> lambda1 ----> lambda2 ----> lambda3
//         rate1         rate2         rate3
// lambda0 + lambda1 + lambda2 + lambda3 = 1
    if( y[3] >= 1.0 )
    {
        int i;
        for( i=0; i<Dimension(); i++ )
            y_prime[i] = 0.0;
        return 0;
    }
    double T       = y[0];
    double lambda1 = y[1];
    double lambda2 = y[2];
    double lambda3 = y[3];
    double lambda0 = 1. - (lambda1+lambda2+lambda3);
    double rate1 = (lambda0<=0.) ? 0.0 : lambda0*TS.Rate1(T);
    double rate2 = (lambda1<=0.) ? 0.0 : lambda1*TS.Rate2(T);
    double rate3 = (lambda2<=0.) ? 0.0 : lambda2*TS.Rate3(T);
    y_prime[1] = rate1 - rate2;
    y_prime[2] = rate2 - rate3;
    y_prime[3] = rate3;
    if( evolution == adiabatic )
    {
        double dQdt  = TS.Q1*rate1+TS.Q2*rate2+TS.Q3*rate3;
        double CV = VT->CV(V0,T);
        y_prime[0] = dQdt/CV;
    }
    else
        y_prime[0] = 0.0;
    return 0;
}

int IntegrateRate::Initialize(double t0, double V, double *y)
{
    V0 = V;
    double T = y[0];
    double dt = 0.01/max(TS.Rate1(T),max(TS.Rate2(T),TS.Rate2(T)));
    dt_min = min(dt_min, 1.e-8*dt);
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
    return y[3];   // lambda3
}
//
//
//
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file     = NULL;
    const char *type     = NULL;
    const char *name     = NULL;
	const char *material = NULL;
    const char *units    = NULL;
    const char *lib      = NULL;
    //
    double T1      =  500.;
    double T2      = 2500.;
    double lambda1 = 0.5;
    int nsteps     = 10;
    double abserr  = 1.e-8;
    double relerr  = 1.e-8;
    IntegrateRate::integrate evolution = IntegrateRate::adiabatic;
    //
    int verbose = 1;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
	    GetVar(lib,lib);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(material,material);
        GetVar(units,units);

        GetVar(T1,T1);
        GetVar(T2,T2);
        GetVar(lambda1,lambda1);
        GetVar(nsteps,nsteps);
        if( !strcmp(*argv,"adiabatic") )
        {
            evolution = IntegrateRate::adiabatic;
            continue;
        }
        if( !strcmp(*argv,"isothermal") )
        {
            evolution = IntegrateRate::isothermal;
            continue;
        }        
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
	}
    if( type==NULL || name==NULL )
        cerr << Error("must specify type and name") << Exit;
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
    EOS_VT *VT = eos->eosVT();
    if( VT == NULL )
        cerr << Error("VT is NULL") << Exit;
    double V0 = eos->V_ref; 
    ThreeStep MT;
    IntegrateRate Rate(evolution,MT,VT);
    lambda_ODE Lambda;
    Rate.abserr = abserr;
    Rate.relerr = relerr;
    //
	cout.setf(ios::left, ios::adjustfield);
    cout << setw(10) << "  T1"
         << " " << setw(10) << " P1"
         << " " << setw(10) << " Cv"
         << " " << setw(10) << " 1/Rate1"
         << " " << setw(10) << " 1/Rate2"
         << " " << setw(10) << " 1/Rate3"
         << " " << setw(10) << " t@lambda1"
         << " " << setw(10) << " T2"
         << " " << setw(10) << " P2"
         << "\n";
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::showpoint);
	cout.setf(ios::scientific, ios::floatfield);
    int i;
    for( i=0; i<=nsteps; i++ )
    {
        double T = T1 + double(i)/nsteps*(T2-T1);
        double P = VT->P(V0,T);
        double Cv = VT->CV(V0,T);
        double y[4] = {T,0.0,0.0,0.0};
        double yp[4];
        double t = 0.0;
        if( Rate.Initialize(t,V0,y) )
            cerr << Error("Rate.Initialize failed") << Exit;
        Rate.F(yp,y,t);
        cout <<        setw(10) << setprecision(4) << T
             << " " << setw(10) << setprecision(4) << P
             << " " << setw(10) << setprecision(4) << Cv
             << " " << setw(10) << setprecision(4) << 1./MT.Rate1(T)
             << " " << setw(10) << setprecision(4) << 1./MT.Rate2(T)
             << " " << setw(10) << setprecision(4) << 1./MT.Rate3(T);
        int ODEstatus = Rate.Integrate(Lambda,lambda1,t,y,yp);
        if( ODEstatus )
            cerr << Error("Rate.Integrate failed with status ")
                 << Rate.ErrorStatus(ODEstatus) << "\n";             
        T = y[0];
        P = VT->P(V0,T);
        cout << " " << setw(10) << setprecision(4) << t
             << " " << setw(10) << setprecision(4) << T
             << " " << setw(10) << setprecision(4) << P
             <<  "\n";
    }
    deleteEOS_VT(VT);
    deleteEOS(eos);
    return 0;
}
