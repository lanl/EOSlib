#include <Arg.h>
#include <LocalMath.h>
#include <ODE.h>
#include "ElasticPlastic1D.h"

class PlasticRate : public ODE
{
public:
    ElasticPlastic1D &EP;
    double V, e;
    double abs_err, rel_err;
    PlasticRate(ElasticPlastic1D *ep, double V0, double e0)
        : ODE(1,512), EP(*ep),V(V0), e(e0)
        { abs_err = 1e-4; rel_err = 1e-2;}
    int F(double *y_prime, const double *y, double eps_pl); // ODE::F   
};
int PlasticRate::F(double *y_prime, const double *y, double t)
{
    if( std::abs(EP.Eps_el(V,e,y)) > 1 )
        return 1;
    return (EP.Rate(V,e,y,y_prime)!=1) ? 1 : 0;   
}
//
#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    //const char *file = "Test.data";
    std::string file_;
    file_ = (getenv("EOSLIB_DATA_PATH") != NULL) ? getenv("EOSLIB_DATA_PATH") : "DATA ENV NOT SET!";
    file_ += "/test_data/Elastic1DTest.data";
    const char * file = file_.c_str();
    
    const char *type = "VonMises1D";
    const char *name = "HMX";
    const char *units = NULL;

    EOS::Init();
    double V = NaN;
    double e = NaN;
    double dt = NaN;
    double eps_pl = -0.1;   // eps < 0 in compression
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

    if( !std::isnan(V) && !std::isnan(e) )
    {
        cout << "EOS\n"
             << "          T = " << eos->T(V,e)         << "\n"
             << "          P = " << eos->P(V,e)         << "\n"  
             << "          S = " << eos->S(V,e)         << "\n"
             << "          c = " << eos->c(V,e)         << "\n";
        ElasticPlastic1D *EP = dynamic_cast<ElasticPlastic1D *>(eos);
        if( EP == NULL )
            cerr << Error("dynamic_cast failed") << Exit;
        EP->Frozen();
        cout << "ExtEOS(V,e), frozen\n"
             << "          T = " << EP->T(V,e)         << "\n"
             << "          P = " << EP->P(V,e)         << "\n"  
             << "          S = " << EP->S(V,e)         << "\n";  
        EP->Equilibrium();
        cout << "ExtEOS(V,e), equilibrium\n"
             << "          T = " << EP->T(V,e)         << "\n"      
             << "          P = " << EP->P(V,e)         << "\n"      
             << "          S = " << EP->S(V,e)         << "\n";      
        EP->Frozen();
        double z[1], zdot[1];
        z[0] = eps_pl;
        cout << "ElasticPlastic1D(V,e,z)\n"
             << "     eps_pl = " << eps_pl             << "\n"
             << "     eps_el = " << EP->Eps_el(V,e, z) << "\n"
             << "          T = " << EP->T(V,e,z)       << "\n"
             << "          P = " << EP->P(V,e, z)      << "\n"        
             << "     Phydro = " << EP->Phydro(V,e, z) << "\n"        
             << "     Pshear = " << EP->Pshear(V,e, z) << "\n"        
             << "       Pdev = " << EP->Pdev(V,e, z)   << "\n"        
             << "          S = " << EP->S(V,e, z)      << "\n"        
             << "      yield: " 
             << "  strength = " << EP->yield_strength(V,e, z)
             << ", function = " << EP->yield_function(V,e, z) << "\n";
        if( EP->Rate(V,e, z, zdot) != 1 )
            cerr << Error("Rate failed") << Exit;            
        cout << "strain_rate = " << zdot[0] << "\n";
        double dt_step;
        int status = EP->TimeStep(V,e,z,dt_step); 
        cout << "TimeStep dt = " << dt_step << ", status " << status << "\n";
        if( std::isnan(dt) )
            dt = dt_step;
        else
            nf = 1;
        double zY[1];
        zY[0] = z[0];
        if( EP->Equilibrate(V,e,zY) )
            cerr << Error("Equilibrate failed") << Exit;
        cout << "equilibrium\n"
             << "     eps_pl = " << zY[0]
             << "  eps_el = " << EP->Eps_el(V,e, zY)
             << ", at Y_func = " << EP->yield_function(V,e, zY) << "\n"
             << "          T = " << EP->T(V,e, zY) << "\n"
             << "          P = " << EP->P(V,e, zY) << "\n"
             << "       Pdev = " << EP->Pdev(V,e, zY) << "\n"
             << "          S = " << EP->S(V,e, zY) << "\n";
        EP->Rate(V,e, zY, zdot);     
        cout << "\tplastic-strain rate = " << zdot[0] << "\n";
     // Exact integration
        PlasticRate PR(EP,V,e);
        double t = 0;
        double y[1], yp[1];
        y[0] = z[0];
        status = PR.Initialize(t, y, 0.1*dt_step);
        if( status )
            cerr << Error("PR.Initialize failed with status ")
                 << PR.ErrorStatus(status) <<Exit;
        t = nf*dt;
        status = PR.Integrate(t,y,yp);
        if( status )
            cerr << Error("PR.Integrate failed with status ")
                 << PR.ErrorStatus(status) <<Exit;
        cout << "ODE integrate to t = " << t << "\n"
             << "\teps_pl = " << y[0]
             << "  eps_el = " << EP->Eps_el(V,e, y) << "\n"
             << "      yield: " 
             << "  strength = " << EP->yield_strength(V,e, y)
             << ", function = " << EP->yield_function(V,e, y) << "\n"
             << "\tplastic-strain rate = " << yp[0]
             << "\n";
     // ElasticPlastic1D->Integrate
        if( (status=EP->Integrate(V,e,z,t)) )
            cerr << Error("Integrate failed") << Exit;
        cout << "ElasticPlastic1D->Integrate status " << status << "\n"
             << "\teps_pl = " << z[0]
             << "  eps_el = " << EP->Eps_el(V,e, z) << "\n"
             << "      yield: " 
             << "  strength = " << EP->yield_strength(V,e, z)
             << ", function = " << EP->yield_function(V,e, z) << "\n";
        EP->Rate(V,e, z, zdot);     
        cout << "\tplastic-strain rate = " << zdot[0] << "\n";       
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
            ElasticPlastic1D *VM = dynamic_cast<ElasticPlastic1D*>(eos);
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
