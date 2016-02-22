#include <Arg.h>

#include <UsUp.h>
#include <EOS_VT.h>

class Eisentrope
{
public:
    double V0;
    double e0;
    double P0;
    double c0;
    double s;
    double Gamma0;
public:
    Eisentrope(UsUp *eos)
    {
        V0 = eos->V_ref;
        e0 = eos->e_ref;
        P0 = eos->P(V0,e0);
        c0 = eos->c0;
        s  = eos->s;
        Gamma0 = eos->G0;
    }
    double Step(double e, double u, double du);
    double dedu(double e, double u);
    double dedu_inf();
    double V(double u);
    double up(double V1);
};
double Eisentrope::V(double u)
{
    return V0*(1.-u/(c0+s*u));
}
double Eisentrope::up(double V)
{
    double eps = 1. - V/V0;
    return c0*eps/(1.-s*eps);
}
double Eisentrope::Step(double e, double u, double du)
{
    //double e1 = e + 0.5*du*dedu(e,u);   // predictor
    //return e + du*dedu(e1,u+0.5*du);    // corrector
    // fourth order Runge-Kutta
    double de1 = 0.5*du*dedu(e,u);
    double de2 = 0.5*du*dedu(e+de1,u+0.5*du);
    double de3 = du*dedu(e+de2,u+0.5*du);
    double de4 = du*dedu(e+de3,u+du);
    return e + (2.*de1+4.*de2+2.*de3+de4)/6.;
}
double Eisentrope::dedu(double e, double u)
{
    double us = c0 + s*u;
    double us2 = us*us;
    double upus = u/us;
    return c0*(P0*V0/us2+upus+Gamma0*((e-e0-P0*V0*upus)/us2-0.5*upus*upus));
}
double Eisentrope::dedu_inf()
{
    return c0/s*(1. -0.5*Gamma0/s);
}



int main(int, char **argv)
{
	ProgName(*argv);
	const char *file = "Test.data";
	const char *type = "UsUp";
	const char *name = "HMX";
	const char *units = NULL;
    int nsteps  = 10;
    double fV = 0.8; // fraction of maximum strain
    int Nmax = 40;
    double fc = 0.1;
    EOS::Init();
	while(*++argv)
	{
		GetVar(file,file);
		GetVar(files,file);
		GetVar(name,name);
		GetVar(units,units);
        
		GetVar(nsteps,nsteps);		
		GetVar(fV,fV);		
		GetVar(fc,fc);		
		GetVar(Nmax,Nmax);		
		ArgError;
	}

	if( file == NULL )
		cerr << Error("Must specify data file") << Exit;

	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;

	EOS *eos = FetchEOS(type,name,db);
	if( eos == NULL )
		cerr << Error("Fetch failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;
    double Vref = eos->V_ref;
    double eref = eos->e_ref;    
    UsUp *usup = static_cast<UsUp*>(eos);
    if( usup == NULL )
		cerr << Error("usup is NULL") << Exit;
    double c0 = usup->c0;
   
    HydroState state0(Vref,eref);
    Isentrope *S = eos->isentrope(state0);
    if( S == NULL )
        cerr << Error("eos->isentrope failed") << Exit;

    Eisentrope E(usup);
    double u[Nmax];
    double e[Nmax];
    double dedu[Nmax];
    u[0] = e[0] = 0.0;
    dedu[0] = E.dedu(e[0],u[0]);
    double du = fc*c0;
    int i;
    for( i=1; i<Nmax;i++ )
    {
        e[i] = E.Step(e[i-1],u[i-1],du);
        u[i] = u[i-1]+du;
        dedu[i] = E.dedu(e[i],u[i]);
        //cout << "i, u,e = " << i << ", " << u[i] << " " << e[i] << "\n";
        /**********************
        double V = E.V(u[i]);
        WaveState wave;
        if( S->V(V,RIGHT,wave) )
            cerr << Error("S->V failed") << Exit;
        double eps = 1. - V/Vref;
        cout << eps << " " << V
                    << " " << wave.e 
                    << " " << e[i]
                    << "\n";
        **********************/     
    }
    /***************
    // accuracy: jump in second derivative
    for( i=1; i<Nmax-1;i++ )
    {
        double d2edu1 = 6.*(e[i-1]-e[i])+2.*(2.*dedu[i]+dedu[i-1])*du;
        double d2edu0 = 6.*(e[i+1]-e[i])-2.*(2.*dedu[i]+dedu[i+1])*du;
        cout << "i=" << i
             << ", du^2*d2e/du2 = " << d2edu1 << ", " << d2edu0
             << ", de/du = " << dedu[i] << "\n";
    }
    ****************/
    double dedu_ln = (E.dedu_inf() - dedu[Nmax-1])*u[Nmax-1];
    cout << "de/du -> " << E.dedu_inf() << "\n";
    double s  = usup->s;
    double Vmin = (1.-1./s)*Vref;
    for( i=0; i<=nsteps; i++ )
    {
        double V = Vref - fV*(double(i)/double(nsteps))*(Vref-Vmin);
        double eps = 1.-V/Vref;
        double uV = E.up(V);
        int j = int(uV/du);
        double eV;
        if( j+1>=Nmax )
            eV = e[Nmax-1] + (uV-u[Nmax-1])*E.dedu_inf()
                           - dedu_ln*log(uV/u[Nmax-1]);
        else
        {
            double x = (uV-u[j])/du;
            eV = (e[j]*(1. +2.*x)+dedu[j]*du*x)*(1.-x)*(1.-x)
                 +x*x*(e[j+1]*(1. +2.*(1.-x))-dedu[j+1]*du*(1.-x));
        }
        WaveState wave;
        if( S->V(V,RIGHT,wave) )
            cerr << Error("S->V failed") << Exit;
        cout << eps << " " << wave.V
                    << " " << wave.e 
                    //<< " " << eapprox
                    << " " << eV
                    << "\n";
    }       


    
/**************************    
    double P0 = eos->P(Vref,eref);
    double K0 = eos->KS(Vref,eref);
    double G0 = eos->FD(Vref,eref);
    double Gamma0 = usup->G0;
    double p3 = (3.*G0/2.-Gamma0/3.)*G0/2.;
    cout << "P0 = " << P0 << ", K0 = " << K0 << "\n";
    cout << "G0 = " << G0 << ", p3 = " << p3 << "\n";
    double T0 = eos->T(Vref,eref);
    double KT = eos->KT(Vref,eref);
    double CV = eos->CV(Vref,eref);
    double N = 2.*G0-1.+(2.*G0-Gamma0)*Gamma0*Gamma0*CV*T0/(Vref*KT);
    double GammaV = Gamma0/Vref;
    int i;
    double e0 = 0;
    double V1 = Vref;
    for( i=0; i<=nsteps; i++ )
    {
        double V = Vref - f*(double(i)/double(nsteps))*(Vref-Vmin);
        double eps = 1.-V/Vref;
        //double eapprox = Vref*eps*(P0+K0*eps*(0.5+eps*(G0/3.+p3/4.*eps)));
        double eHayes = e0 + (P0-KT/N-CV*T0*GammaV)*(V1-V)
                        + (KT*V1/N/(N-1.))*(pow(V1/V,N-1.)-1.)
                        + CV*T0*(exp(GammaV*(V1-V))-1.);
        P0 = eos->P(V,eHayes);
        KT = eos->KT(V,eHayes);
        G0 = eos->FD(V,eHayes);
        T0 = T0*exp(GammaV*(V1-V));
        N  = 2.*G0-1.+(2.*G0-Gamma0)*Gamma0*Gamma0*CV*T0/(V*KT);
        V1 = V;
        e0 = eHayes;
        
        WaveState wave;
        if( S->V(V,RIGHT,wave) )
            cerr << Error("S->V failed") << Exit;
        cout << eps << " " << wave.V
                    << " " << wave.e 
                    //<< " " << eapprox
                    << " " << eHayes
                    << "\n";
    }       
**************************/    
    delete S;
    deleteEOS(eos);
    return 0;
}
