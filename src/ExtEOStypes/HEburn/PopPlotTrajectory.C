#include <Arg.h>
#include <String.h>
//
#include "HEburn.h"
#include "FFrate.h"
//
class trajectory : public ODE
{
public:
    trajectory(FFrate &rate);
    int F(double *y_prime, const double *y_xi, double x); // ODE::F 
    int t(double &p, double &x, double &t);
    // Pop plot parameters: log_10(P/Pref) = a - b*log_10(x/xref)
    double a,b, aln;
    double X(double p);
    double P(double x);
    // Reactive hugoniot: us = c0 +s*u
    double c0,s;
    double Us(double p);
    double V0;
    double Pcj,ucj,Dcj, xcj;
    //
    double y[0];
};
trajectory::trajectory(FFrate &rate) : ODE(1,512)
{
    a   = rate.a;
    b   = rate.b;
    aln = a*log(10.);
    c0  = rate.hug.c0;
    s   = rate.hug.s;
    Pcj = rate.hug.CJ.P;
    ucj = rate.hug.CJ.u;
    Dcj = rate.hug.CJ.us;
    V0  = rate.hug.ws0.V;
    xcj = X(Pcj);
    //
    y[0] = 0.;
    double x0 = -xcj;
    double dx = X(0.98*Pcj)-xcj;
    int status = Initialize(x0, y, -dx);
    if( status )
        cerr << Error("trajectory, Initialize failed with status ")
             << ErrorStatus(status) << Exit;
}
double trajectory::X(double p)
{
        return exp((aln - log(p))/b);
}
double trajectory::P(double x)
{
        return exp(aln-b*log(x));
}
double trajectory::Us(double p)
{
    return 0.5*(c0 + sqrt(c0*c0+4.*s*p*V0));
}
int trajectory::F(double *y_prime, const double *y_xi, double x)
{
    // dt/dx = 1/us
    double  p = P(-x);
    y_prime[0] = 1/Us(p); 
    return 0;
}
class P_ODE : public ODEfunc
{
public:
    P_ODE(trajectory *t) : T(*t) {}
    trajectory &T;
    double f(double x, const double *y, const double *yp);
};
double P_ODE::f(double x, const double *y, const double *yp)
{
    return T.P(-x);    
}
int trajectory::t(double &p, double &x, double &t)
{
    P_ODE Pt(this);
    int status = Integrate(Pt,p,x,y);
    t = y[0];
    return status;
}
//
int main(int, char **argv)
{
    ProgName(*argv);
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/HE.data";
    const char * file = file_.c_str();
 

    //const char *file     = "HE.data";
    const char *type     = "HEburn";
    const char *name     = "PBX9501.sam.FF";
	const char *material = NULL;
    const char *units    = NULL;
    const char *lib      = "../../lib/Linux";
    double Pmin =  2.;
    int nsteps  = 20;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
	    //GetVar(material,material);
	    GetVar(lib,lib);
        GetVar(units,units);
        GetVar(Pmin,Pmin);
        GetVar(nsteps,nsteps);
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
    trajectory T(*Rate);
    double Pcj = T.Pcj;
	cout.setf(ios::left, ios::adjustfield);
    cout        << setw(13) << "# P"
         << " " << setw(13) << "   us"
         << " " << setw(13) << "   x"
         << " " << setw(13) << "   t"
         << "\n";
	cout.setf(ios::showpoint);
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::scientific, ios::floatfield);
    int i;
    double P,x,t,us;
    double xlast = 0.;
    double tlast = 0.;
    for( i=0; i<= nsteps; i++ )
    {
        P = Pcj - double(i)/double(nsteps) * (Pcj-Pmin);
        us = T.Us(P);
        int status = T.t(P,x,t);
        if ( status )
             cerr << Error("T.t failed with status ")
                  << T.ErrorStatus(status) << Exit;        
        cout        << setw(13) << setprecision(6) << P
             << " " << setw(13) << setprecision(6) << us
             << " " << setw(13) << setprecision(6) << x
             << " " << setw(13) << setprecision(6) << t;
        /***  consistency check
        if( xlast )
        {
            us = (x-xlast)/(t-tlast);
            cout << setw(13) << setprecision(6) << us;
        }
        ****/
        cout << "\n";
        xlast = x;
        tlast = t;
    }
    deleteEOS(eos);
    return 0;
}
