#include <Arg.h>
#include <LocalMath.h>
#include "HEburn.h"
#include "PTequilibrium.h"
//
#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/HE.data";
    const char * file = file_.c_str();
 

    //const char *file = "HE.data";
    const char *type = "HEburn";
    const char *name = "PBX9501.BM";
    const char *units = NULL;

    EOS::Init();
    double V = 6.31361524600090e-01;
    double e = 1.01320478005067e+01;
    double lambda = 1.21474498098123e-01;
    double dt = 0.01;



    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        GetVar(V,V);
        GetVar(e,e);
        GetVar(lambda,lambda);
        GetVar(dt,dt);
        
        ArgError;
    }
    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;
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
    //
    double z[1], zdot[1];
    z[0] = lambda;
    double P = HE->P(V,e,z);
    double T = HE->T(V,e,z);
    if( HE->Rate(V,e,z,zdot) != 1 )
        cerr << Error("HE->Rate failed" ) << Exit;
    cout << "P T rate = " << P
         << ", " << T
         << ", " << zdot[0]
         << "\n";
    cout << "  V1,e1 = " << PT->V1 << ", " << PT->e1 << "\n"
         << "  V2,e2 = " << PT->V2 << ", " << PT->e2 << "\n";
    //
    double Dt = 0.;;
    int status = HE->TimeStep(V,e,z, Dt);
    if( status < 0 )
        cerr << Error("TimeStep failed") << Exit;
    cout << "status " << status << ", dt = " << Dt << "\n";

    if( HE->Integrate(V,e,z,dt) )
        cerr << Error("HE->Integrate failed") << Exit;
    cout << "Integrate: lambda = " << z[0] << "\n";
    deleteEOS(eos);
    return 0;
}
