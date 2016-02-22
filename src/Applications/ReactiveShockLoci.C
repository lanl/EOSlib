#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include <HEburn.h>
#include <FFrate.h>
//
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file     = NULL;
    const char *type     = "HEburn";
    const char *name     = NULL;
    const char *units    = NULL;
    const char *lib      = NULL;
    double epsilon = 1.e-8;
    double lambda0 = 0.;
    double lambda1 = 1.;

    int nsteps = 20;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
	    GetVar(lib,lib);
        GetVar(units,units);
        GetVar(nsteps,nsteps);
        GetVar(epsilon,epsilon);
        GetVar(lambda0,lambda0);
        GetVar(lambda1,lambda1);
        ArgError;
    }
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
	if( name == NULL )
		cerr << Error("Must specify name") << Exit;
    if( lib )
#ifdef LINUX
        setenv("SharedLibDirReactiveHugoniot::EOS",lib,1);
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
        cerr << Error("dynamic_cast failed HEburn") << Exit;
    FFrate *FF = dynamic_cast<FFrate *>(HE->rate);
    if( FF == NULL )
        cerr << Error("dynamic_cast failed for FFrate") << Exit;
    //
    ReactiveHugoniot &locus = FF->hug;
    locus.epsilon = epsilon;
    cout << "#CJstate:  V=" << locus.CJ.V 
         <<         "  P=" << locus.CJ.P
         <<         " us=" << locus.CJ.us
         << "\n";
	cout.setf(ios::left, ios::adjustfield);
    cout        << setw(13) << "#  lambda"
         << " " << setw(13) << "   V"
         << " " << setw(13) << "   e"
         << " " << setw(13) << "   P"
         << " " << setw(13) << "   u"
         << " " << setw(13) << "   c"
         << " " << setw(13) << "   us"
         << " " << setw(13) << "   u+c-us"
         << " " << setw(13) << "   x"
         << " " << setw(13) << "   Rate"
         << " " << setw(13) << "   FFrate"
         << " " << setw(13) << " thermicity"
         << " " << setw(13) << " g1"
         << " " << setw(13) << " V0(dP/dx)pop"
         << "\n";
	cout.setf(ios::showpoint);
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::scientific, ios::floatfield);
    int i;
    for( i=0; i<=nsteps; i++ )
    {
        double lambda = lambda0 + (double(i)/double(nsteps))*(lambda1-lambda0);
        int status = locus.Lambda(lambda);
        if( status )
            cerr << Error("locus.Lambda failed with status: \n")
                 << locus.ErrorStatus(status) << Exit;
        double c =  locus.c(locus.ws.V,locus.ws.e,lambda);
        double du = locus.ws.u + c - locus.ws.us;
        // consistency check
        double P = locus.P(locus.ws.V,locus.ws.e,lambda);
        if( abs(P-locus.ws.P) > 1e-3*locus.ws.P )
            cout << "Ps, P(V,e) = " << locus.ws.P << "," << P << "\n";
        //
        cout        << setw(13) << setprecision(6) << lambda
             << " " << setw(13) << setprecision(6) << locus.ws.V
             << " " << setw(13) << setprecision(6) << locus.ws.e
             << " " << setw(13) << setprecision(6) << locus.ws.P
             << " " << setw(13) << setprecision(6) << locus.ws.u
             << " " << setw(13) << setprecision(6) << c
             << " " << setw(13) << setprecision(6) << locus.ws.us
             << " " << setw(13) << setprecision(6) << du;
        double x = FF->Xpop(locus.ws.P);
        double Rate = FF->Rate(locus.ws.P);
        double FFrate = FF->Rate(0.,locus.ws.P);
        double dP, dT;
        if( locus.ddlambda(locus.ws.V,locus.ws.e,lambda,dP,dT) )
            cerr << Error("locus.dlambda failed\n"); 
        double thermicity = locus.ws.V*dP/(c*c);
        cout << " " << setw(13) << setprecision(6) << x
             << " " << setw(13) << setprecision(6) << Rate
             << " " << setw(13) << setprecision(6) << FFrate
             << " " << setw(13) << setprecision(6) << thermicity;
        double z = c/(locus.ws.us-locus.ws.u);
        double z2 = z*z;
        double den = 1. + (locus.ws.u/locus.ws.us)*locus.dUsdUp(locus.ws.u);
        double g1 = (z2-1.)/(1.+z2/den);
        double dPdx = locus.ws0.V*FF->b*P/x;
        cout << " " << setw(13) << setprecision(6) << g1
             << " " << setw(13) << setprecision(6) << dPdx
             << "\n";
    }
    deleteEOS(eos);
    return 0;
}
