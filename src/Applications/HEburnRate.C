#include <Arg.h>
#include <HEburn.h>
#include <HotSpotRate.h>
#include <HotSpotRateExp.h>
//
// Calculates reaction rate on Hugoniot locus
// for HEburn material type
//
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
    double P1,P2 = -1.;     // range on shock locus  
    double lambda = 0.0;   // calculate adiabatic burn time
    int nsteps = 20;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
	    GetVar(lib,lib);
        GetVar(name,name);
        //GetVar(type,type);
        GetVar(material,material);
        GetVar(units,units);
        GetVar(P1,P1);
        GetVar(P2,P2);
        GetVar(lambda,lambda);
        GetVar(nsteps,nsteps);
        ArgError;
    }
    if( P1 <= 0. || P2 <= 0. )
        cerr << Error("Must specify P1 and P2") << Exit;    
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
    HErate  *rate = HE->rate;
    HotSpotRate    *HSrate    = dynamic_cast<HotSpotRate *>(rate);
    HotSpotRateExp *HSrateExp = dynamic_cast<HotSpotRateExp *>(rate);
    //
    double Vref = eos->V_ref; 
    double eref = eos->e_ref;
    HydroState state0(Vref,eref);
    //
    Hugoniot   *hug = HE->shock(state0);
    if( hug==NULL )
        cerr << Error("HE->shock failed" ) << Exit;
	cout.setf(ios::left, ios::adjustfield);
    cout        << setw(13) << "# V"
         << " " << setw(13) << "   e"
         << " " << setw(13) << "   u"
         << " " << setw(13) << "   us"
         << " " << setw(13) << "   c"
         << " " << setw(13) << "   P"
         << " " << setw(13) << "   T"
         << " " << setw(13) << "   Rate";
    if( lambda > 0.0 )
        cout << " " << setw(13) << "   t@lambda";
    cout << "\n";
	cout.setf(ios::showpoint);
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::scientific, ios::floatfield);
    double *z = &(*HE->Zref());
    double *zdot = new double[HE->N()];
    int i;
    for( i=0; i<=nsteps; i++ )
    {
        double x = double(i)/double(nsteps);
        double P = (1.-x)*P1 + x*P2;
        WaveState wave;
        if( hug->P(P,RIGHT,wave) )
            cerr << Error("hug->P failed") << Exit;    
        double c = HE->c(wave.V,wave.e,z);
        double T = HE->T(wave.V,wave.e,z);
        //if( HE->Rate(wave.V,wave.e,z,zdot) < 0 )
        //    cerr << Error("HE->Rate failed") << Exit;
        // change only requires ExtEOS rather than HEburn
        double rate;
        if( HSrate )
        {
           z[0] = P;
           z[1] = 0.0;  // ts
           z[2] = 0.0;  // lambda
           z[3] = 0.0;  // s
           if( HE->var("ddt_s",wave.V,wave.e,z,rate) )
                cerr << Error("HE->Rate failed") << Exit;
        }
        else if( HSrateExp )
        {
           z[0] = P;
           z[1] = T;
           z[2] = 0.0;  // ts
           z[3] = 0.0;  // lambda
           z[4] = 0.0;  // s
           if( HE->var("ddt_s",wave.V,wave.e,z,rate) )
                cerr << Error("HE->Rate failed") << Exit;
        }
        else
        {
            if( HE->var("ddt_lambda",wave.V,wave.e,z,rate) )
                cerr << Error("HE->Rate failed") << Exit;
        }
        cout        << setw(13) << setprecision(6) << wave.V
             << " " << setw(13) << setprecision(6) << wave.e
             << " " << setw(13) << setprecision(6) << wave.u
             << " " << setw(13) << setprecision(6) << wave.us
             << " " << setw(13) << setprecision(6) << c
             << " " << setw(13) << setprecision(6) << wave.P
             << " " << setw(13) << setprecision(6) << T
             //<< " " << setw(13) << setprecision(6) << zdot[0]
             << " " << setw(13) << setprecision(6) << rate;
        if( lambda > 0.0 )
        {
            double t = HE->Dt(wave.V,wave.e,z,lambda);
            cout << setw(13) << setprecision(6) << t;
        }
        cout << "\n";
    }
    delete [] zdot;
    delete hug;
    deleteEOS(eos);
    return 0;
}
