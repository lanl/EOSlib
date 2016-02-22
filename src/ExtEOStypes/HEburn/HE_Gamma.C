#include <Arg.h>
#include <LocalMath.h>
#include "HEburn.h"

int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "HE.data";
    const char *type = "HEburn";
    const char *name = "PBX9501.BM";
    const char *units = NULL;
    const char *lib   = "../../lib/Linux";
    int nsteps  = 10;
    double V_min = 0.3;
    double V_max = 0.;
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        GetVar(lib,lib);
        //
        GetVar(nsteps,nsteps);
        GetVar(Vmin,V_min);
        GetVar(Vmax,V_max);
        ArgError;
    }
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
#ifdef LINUX
    setenv("SharedLibDirEOS",lib,1);
#else
    putenv(Cat("SharedLibDirEOS=",lib));
#endif
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
    PTequilibrium *PT = HE->HE;
    EOS *reactants = PT->eos1;
    EOS *products  = PT->eos2;
    ///
    double Vref = eos->V_ref; 
    double eref = eos->e_ref;
    if( V_max <= 0. )
        V_max = Vref;
    HydroState state0(Vref,eref);
    Isentrope *Ir = reactants->isentrope(state0);
    if( Ir == NULL )
        cerr << Error("reactants->isentrope failed") << Exit;
    Isentrope *Ip = products->isentrope(state0);
    if( Ip == NULL )
        cerr << Error("products->isentrope failed") << Exit;        
	//
    cout.setf(ios::left, ios::adjustfield);
    cout        << setw(13) << "# V"
         << " " << setw(13) << "  Gamma_r"
         << " " << setw(13) << "  Gamma_p"
         << "\n";    
	cout.setf(ios::showpoint);
	cout.setf(ios::right, ios::adjustfield);
	cout.setf(ios::scientific, ios::floatfield);
    //
    int i;
    WaveState state_r;
    WaveState state_p;
    for( i=0; i<=nsteps; i++ )
    {
        double V = V_min + double(i)/double(nsteps) * (V_max-V_min);
        if( Ir->V(V,RIGHT,state_r) )
            cerr << Error("Ir.V failed") << Exit;
        if( Ip->V(V,RIGHT,state_p) )
            cerr << Error("Ip.V failed") << Exit;
        double Gamma_r = reactants->Gamma(V,state_r.e);
        double Gamma_p = products->Gamma(V,state_p.e);
        cout        << setw(13) << setprecision(6) << V
             << " " << setw(13) << setprecision(6) << Gamma_r
             << " " << setw(13) << setprecision(6) << Gamma_p
             << "\n";
    }
    deleteEOS(reactants);
    deleteEOS(products);
    return 0;
}
