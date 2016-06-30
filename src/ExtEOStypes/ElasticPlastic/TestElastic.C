#include <Arg.h>

#include "ElasticPlastic.h"

#define NaN EOS_VT::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "Test.data";
    const char *type = "Shear1";
    const char *name = "HMX";
    const char *units = NULL;

    EOS_VT::Init();
    double V = NaN;
    double T = NaN;
    double eps_el = NaN;
    
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        GetVar(V,V);
        GetVar(T,T);
        GetVar(eps_el,eps_el);
        
        ArgError;
    }

    if( file == NULL )
        cerr << Error("Must specify data file") << Exit;

    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;

    EOS_VT *eos = FetchEOS_VT(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;

    if( !std::isnan(V) && !isnan(T) )
    {
        double P = eos->P(V,T);
        cout << "P = " << P << "\n";
        double e = eos->e(V,T);
        cout << "e = " << e << "\n";
        double S = eos->S(V,T);
        cout << "S = " << S << "\n";
        double c = eos->c(V,T);
        cout << "c = " << c << "\n";

        Elastic_VT *elastic = dynamic_cast<Elastic_VT *>(eos);
        if( elastic == NULL )
            cerr << Error("dynamic_cast failed") << Exit;
        EOS_VT *hydro = elastic->Hydro();
        double *z_f = elastic->z_f(V,T);
        cout << "eps_el(V,T) = " << z_f[0] << "\n";
        if( !std::isnan(eps_el) )
        {
            cout << "at eps_el = " << eps_el << "\n";
            z_f[0] = eps_el;
        }
        cout << "P_hydro = " << hydro->P(V,T) << "\n";
        cout << " stress = " << elastic->stress(V,T, z_f[0]) << "\n";
        cout << "P_total = " << elastic->P(V,T, z_f) << "\n";
        deleteEOS_VT(hydro);
    }
    else
    {
        const char *info = db.BaseInfo("EOS_VT");
        if( info == NULL )
            cerr << Error("No EOS_VT info" ) << Exit;
        cout << "Base info\n"
             << info << "\n";

        info = db.TypeInfo("EOS_VT",type);
        if( info == NULL )
            cerr << Error("No type info" ) << Exit;
        cout << "Type info\n"
             << info << "\n";      
        /*******  Test Duplicate 
            Elastic_VT *elastic = dynamic_cast<Elastic_VT *>(eos);
            double z[1];
            z[0] = 0.52;
            ExtEOS_VT *eos1 = elastic->Duplicate(z);
            eos1->Print(cout);
            deleteExtEOS_VT(eos1);
        *********/
        eos->PrintAll(cout);
        /********  Test conversion Elastic_VT -> ElasticEOS 
            Elastic_VT *VT = dynamic_cast<Elastic_VT *>(eos);
            ElasticEOS eos_ve(*VT);
            eos_ve.PrintAll(cout << "\n*** ElasticEOS ***\n");
            double V = eos_ve.V_ref;
            double e = eos_ve.e_ref;
            cout << " P = " << eos_ve.P(V,e) << "\n";
            cout << " T = " << eos_ve.T(V,e) << "\n";
        *********/          
        
    }
    deleteEOS_VT(eos);

    return 0;
}

