#include <Arg.h>
#include "ViscousProfile.h"
//
//
//
int main(int, char **argv)
{
    ProgName(*argv);
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/ApplicationsHE.data";//this could be HE.data or ApplicationsHE.dtaa  ???
    const char * file = file_.c_str();
    //const char *file     = "HE.data";
    const char *type     = "HEburn";
    const char *name     = "PBX9501.sam.FF";
	const char *material = NULL;
    const char *units    = NULL;
    const char *lib      = NULL;
    double epsilon = 1.e-6;     // accuracy for ODE
    double tol     = 1.e-4;     // tolerance for bisection routine for nu
    double lambda_max = 0.999;  // end of profile
    //
    double nu      = 1;     // viscous coefficient (dimesionless)
    double len     = 0.1;   // length scale
    double r       = 0.25;  // ratio of quadratic to linear viscous coefficient
    double Pvn     = 0.;    // pressure at VN spike
    double Umin    = 0.;    // minimum velocity at start of profile
                            // cutoff exponential tail
    double P0      = 0.1;   // origin for xi0
    double dP      = 0.5;   // pressure step
    int nsteps = 20;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
	    //GetVar(material,material);
	    GetVar(lib,lib);
        GetVar(units,units);
        GetVar(nsteps,nsteps);
        GetVar(epsilon,epsilon);
        GetVar(tol,tol);
        GetVar(lambda_max,lambda_max);
        GetVar(nu,nu);
        GetVar(len,len);
        GetVar(r,r);
        GetVar(Umin,Umin);
        GetVar(P0,P0);
        GetVar(dP,dP);
        ArgError;
    }
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
	if( material )
	{
	    if( type || name )
		    cerr << Error("Can not specify material plus name and type")
	             << Exit;
	    if( TypeName(material,type,name) )
		    cerr << Error("syntax error, material = ") << material
		         << Exit;
	}
    if( type==NULL || name==NULL )
        cerr << Error("must specify type and name or material") << Exit;
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
        cerr << Error("dynamic_cast failed for HEburn") << Exit;
    FFrate *Rate = dynamic_cast<FFrate *>(HE->rate);
    if( Rate == NULL )
        cerr << Error("dynamic_cast failed for FFrate") << Exit;
    ViscousProfile profile(Rate,Pvn,Umin);    
    if( profile.status != 7 )
        cerr << Error("ViscousProfile constructor failed") << Exit;
    profile.epsilon = epsilon;
    int status;
    if( (status=profile.Reinit(nu,len,r)) )
        cerr << Error("reinit failed for nu = ") << nu
             << " with status " << profile.ErrorStatus(status)
             << Exit;
    int i;
    double xi;
    if( (status=profile.Lambda(lambda_max,xi)) )
         cerr << Error("profile.Lambda failed with status ")
              << profile.ErrorStatus(status) << Exit;
    //  find bounds on nu
    double nu1, nu2;
    if( profile.y[0] > profile.ws_cj.u )
    {
        nu1 = 2.*nu;
        nu2 = 0.0;
        for( i=0; i<32; i++, nu1*=2. )
        {
            if( (status=profile.Reinit(nu1)) )
                cerr << Error("reinit failed for nu = ") << nu1
                     << " with status " << profile.ErrorStatus(status)
                     << Exit;
            if( (status=profile.Lambda(lambda_max,xi)) )
                 cerr << Error("profile.Lambda failed with status ")
                      << profile.ErrorStatus(status) << Exit;
            if( profile.y[0] < profile.ws_cj.u )
            {
                nu2 = nu1;
                nu1 *= 0.5;
                break;
            }
        }
        if( nu2 == 0.0 )
            cerr << Error("Bound failed, nu2=0") << Exit;
    }
    else
    {
        nu1 = 0.0;
        nu2 = 0.5*nu;
        for( i=0; i<32; i++, nu2*=0.5 )
        {
            if( (status=profile.Reinit(nu2)) )
                cerr << Error("reinit failed for nu = ") << nu2
                     << " with status " << profile.ErrorStatus(status)
                     << Exit;
            if( (status=profile.Lambda(lambda_max,xi)) )
                 cerr << Error("profile.Lambda failed with status ")
                      << profile.ErrorStatus(status) << Exit;
            if( profile.y[0] > profile.ws_cj.u )
            {
                nu1 = nu2;
                nu2 *= 2.;
                break;
            }
        }
        if( nu1 == 0.0 )
            cerr << Error("Bound failed, nu1=0") << Exit;
    } 
    //  Bisection search for nu_cj
    for( i=0; i<32; i++ )
    {
        nu = 0.5*(nu1+nu2);
        if( (status=profile.Reinit(nu)) )
            cerr << Error("reinit failed for nu = ") << nu
                 << " with status " << profile.ErrorStatus(status)
                 << Exit;
        if( (status=profile.Lambda(lambda_max,xi)) )
             cerr << Error("profile.Lambda failed with status ")
                  << profile.ErrorStatus(status) << Exit;
        if( profile.y[0] > profile.ws_cj.u )
            nu1 = nu;
        else
            nu2 = nu;
        if( nu2-nu1 < tol*nu1 )
            break;
    }
    nu = 0.5*(nu1+nu2);
    cout << "nu " << nu << "\n";
    profile.Reinit(nu);
    profile.Header(cout,P0) << "\n";
    xi  = 1.;  // upstream
    profile.Print(cout,xi) << "\n";
    xi = 0.0;
    profile.Print(cout,xi) << "\n";
    double lambda = 1./double(nsteps);
    if( (status=profile.Lambda(lambda,xi)) ) 
         cerr << Error("profile.lambda1 failed with status ")
              << profile.ErrorStatus(status) << Exit;
    double P1 = profile.p;
    if( dP <= 0.0 )
        dP = profile.ws_s.P/nsteps;
    double p;
    for( p=profile.p0; p<P1; p+=dP )
    {
        if( (status=profile.P(p,xi)) )
             cerr << Error("profile.P failed with status ")
                  << profile.ErrorStatus(status) << Exit;
        profile.Print(cout,xi) << "\n";
    }    
    for( i=1; i<nsteps; i++ )
    {
        lambda = double(i)/double(nsteps);
        if( (status=profile.Lambda(lambda,xi)) )
             cerr << Error("profile.Lambda failed with status ")
                  << profile.ErrorStatus(status) << Exit;
        profile.Print(cout,xi) << "\n";
    }
    double xi_last = xi;
    double  t_last = profile.y[2];    
    lambda = lambda_max;
    if( (status=profile.Lambda(lambda,xi)) )
         cerr << Error("profile.Lambda failed with status ")
              << profile.ErrorStatus(status) << Exit;
    profile.Print(cout,xi) << "\n";
    double t = profile.y[2];
    xi += xi-xi_last;
     t += t- t_last;
    lambda = 1.0;
    profile.Print(cout,xi,profile.ws_s.u,lambda,t) << "\n";
    deleteEOS(eos);
    return 0;
}
