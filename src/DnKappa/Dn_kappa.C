#include <Arg.h>
#include <EOS.h>
#include <DnKappa.h>
using namespace std;

int main(int, char **argv)
{
    ProgName(*argv);
    // material
    const char *files    = "test.data";    
    const char *type     = "ScaledDn";
    const char *name     = "PBX9502";
    const char *material = NULL;
    const char *units    = "hydro::std";
    //
    double kappa1 = 0.0;
    double kappa2 = 0.0;
    double T0   = 0.0;
    double rho0 = 0.0;
    int nsteps = 20;
// process command line arguments
    while(*++argv)
    {
        // material
        GetVar(file,files);
        GetVar(files,files);
        GetVar(type,type);
        GetVar(name,name);
        GetVar(material,material);
        GetVar(units,units);
        //
        GetVar(kappa1,kappa1);
        GetVar(kappa2,kappa2);
        GetVar(rho0,rho0);
        GetVar(T0,T0);
        GetVar(nsteps,nsteps);
        //
        ArgError;
    }
    cout.setf(ios::showpoint);
    cout.setf(ios::scientific, ios::floatfield);
	if( files == NULL )
		cerr << Error("Must specify data file") << Exit;

// fetch material
	DataBase db;
	if( db.Read(files) )
		cerr << Error("Read failed" ) << Exit;

    DnKappa *PBX = FetchDnKappa(type,name, db);
	if( PBX == NULL )
		cerr << Error("Fetch failed") << Exit;
    if( units && PBX->ConvertUnits(units, db) )
		cerr << Error("ConvertUnits failed") << Exit;
    if( rho0 > 0.0 && T0 > 0.0 )
    {
        if( PBX->InitState(rho0,T0) )
		    cerr << Error("PBX->InitState failed") << Exit;
    }
    if( kappa1 == kappa2 && nsteps>0 ) kappa2 = PBX->kappa_max;
    //
    Format k_form(11,4);
    Format D_form(11,4);
    cout <<        Center(k_form,"kappa")
         << " " << Center(D_form,"Dn")
         << "\n";
    const Units *u = PBX->UseUnits();
    if( u )
    {
        char *invlen = Cat("1/",u->Unit("length"));
        cout <<         Center(k_form,invlen)
             << " " <<  Center(D_form,u->Unit("velocity"))
             << "\n";
        delete [] invlen;
    }
    int i;
    for( i=0; i<=nsteps; i++ )
    {
        double kappa = kappa1 + double(i)/double(nsteps) *(kappa2-kappa1);
        double Dn = PBX->Dn(kappa);
        cout  <<        k_form << kappa
              << " " << D_form << Dn
              << "\n";
    }    
    deleteDnKappa(PBX);
    return 0;
}
