#include <Arg.h>

#include "ElasticPlastic.h"

#define NaN EOS_VT::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "Test.data";
    const char *type = "VonMises";
    const char *name = "HMX";
    const char *units = NULL;

    EOS::Init();
    double V = NaN;
    double e = NaN;
    double dt = NaN;
    double eps_pl = 0.1;
    double nf = 20;
    
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        GetVar(V,V);
        GetVar(e,e);
        GetVar(eps_pl,eps_pl);
        GetVar(dt,dt);
        GetVar(nf,nf);
        
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

    if( !isnan(V) && !isnan(e) )
    {
        double P = eos->P(V,e);
        cout << "P = " << P << "\n";
        double T = eos->T(V,e);
        cout << "T = " << T << "\n";
        double S = eos->S(V,e);
        cout << "S = " << S << "\n";
        double c = eos->c(V,e);
        cout << "c = " << c << "\n";

        ElasticPlastic *EP = dynamic_cast<ElasticPlastic *>(eos);
        if( EP == NULL )
            cerr << Error("dynamic_cast failed") << Exit;
        double z[1], zdot[1];
        z[0] = eps_pl;
        cout << "          T = " << EP->T(V,e,z)       << "\n";
        cout << "     eps_el = " << EP->eps_el(V,e, z) << "\n";
        cout << "      shear = " << EP->shear(V,e, z)  << "\n";        
        cout << "      yield: " << EP->yield_surf(V,e, z)
             << ", " << EP->yield_func(V,e, z) << "\n";
        EP->Rate(V,e, z, zdot);     
        cout << "strain_rate = " << zdot[0] << "\n";
        double dt_step;
        int status = EP->TimeStep(V,e,z,dt_step); 
        cout << "TimeStep dt = " << dt_step << ", status " << status << "\n";
        if( isnan(dt) )
            dt = dt_step;
        else
            nf = 1;
        double zY[1];
        zY[0] = z[0];
        if( EP->Equilibrate(V,e,zY) )
            cerr << Error("Equilibrate failed") << Exit;
        cout << "eps_pl      = " << zY[0]
             << "     eps_el = " << EP->eps_el(V,e, zY)
             << ", at Y_func = " << EP->yield_func(V,e, zY) << "\n";
        int i;
        if( (status=EP->Integrate(V,e,z,nf*dt)) )
            cerr << Error("Integrate failed") << Exit;
        cout << "Integrate status " << status << ", eps_pl = " << z[0] << "\n";
        cout << "     eps_el = " << EP->eps_el(V,e, z) << "\n";
        cout << "      yield: " << EP->yield_surf(V,e, z)
             << ", " << EP->yield_func(V,e, z) << "\n";
        EP->Rate(V,e, z, zdot);     
        cout << "strain_rate = " << zdot[0] << "\n";       
    }
    else
    {
        const char *info = db.BaseInfo("EOS");
        if( info == NULL )
            cerr << Error("No EOS info" ) << Exit;
        cout << "Base info\n"
             << info << "\n";

        info = db.TypeInfo("EOS",type);
        if( info == NULL )
            cerr << Error("No type info" ) << Exit;
        cout << "Type info\n"
             << info << "\n";
        /*******  Test Duplicate
            ElasticPlastic *VM = dynamic_cast<ElasticPlastic *>(eos);
            double z[1];
            z[0] = -0.1;
            ExtEOS *eos1 = VM->Duplicate(z);
            eos1->Print(cout);
            deleteExtEOS(eos1);
        *********/
        eos->PrintAll(cout);
    }
    deleteEOS(eos);

    return 0;
}

