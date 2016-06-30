#include <Arg.h>
#include <LocalMath.h>
#include "ArrheniusHE.h"
//
#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "Test.data";
    const char *type = "ArrheniusHE";
    const char *name = "HMX";
    const char *units = NULL;

    EOS::Init();
    double V = NaN;
    double e = NaN;
    double lambda = NaN;

    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        GetVar(V,V);
        GetVar(e,e);
        
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
    cout << "eos->ref_count = " << eos->RefCount() << "\n";
    ArrheniusHE *HE = dynamic_cast<ArrheniusHE *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    EOS *reactants = HE->reactants();
    EOS *products  = HE->products();
    cout << "reactants->ref_count = " << reactants->RefCount() << "\n";
    cout << "products->ref_count = "  << products->RefCount()  << "\n";    
    if( !std::isnan(lambda) )
        HE->set_lambda(lambda);
    lambda = HE->get_lambda();

    if( !std::isnan(V) && !isnan(e) )
    {
        cout << "EOS\n"
             << "          T = " << eos->T(V,e)         << "\n"
             << "          P = " << eos->P(V,e)         << "\n"  
             << "          S = " << eos->S(V,e)         << "\n"
             << "          c = " << eos->c(V,e)         << "\n";
        double t_induction = HE->Dt(V,e,HE->z_f(V,e),0.9);
        cout << "t induction = " << t_induction << "\n";        
        HE->Frozen();
        cout << "ExtEOS(V,e), frozen\n"
             << "          T = " << HE->T(V,e)         << "\n"
             << "          P = " << HE->P(V,e)         << "\n"  
             << "          S = " << HE->S(V,e)         << "\n";  
        HE->Equilibrium();
        cout << "ExtEOS(V,e), equilibrium\n"
             << "          T = " << HE->T(V,e)         << "\n"      
             << "          P = " << HE->P(V,e)         << "\n"      
             << "          S = " << HE->S(V,e)         << "\n";      
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
        eos->PrintAll(cout);
        ExtEOS *xeos = static_cast<ExtEOS*>(eos);
        cout << "IDOF name = " << xeos->Zref()->name << "\n";
        cout << "index of lambda = " 
              << xeos->Zref()->var_index("lambda") << "\n";
        cout << "index of dummy = " 
              << xeos->Zref()->var_index("dummy") << "\n";
        
        double Vref = eos->V_ref; 
        double eref = eos->e_ref;
        double lambda_ref = HE->lambda_ref();
        cout << "\nReference state: V, e, lambda = " << Vref << ", " << eref
             << ", " << lambda_ref << "\n";
        double P, T, S;
        double z[1], zdot[1];
        z[0] = 0.0;     // lambda = 0 (reactants)
        cout << "ambient state for reactants\n";
        P = HE->P(Vref,eref,z);
        T = HE->T(Vref,eref,z);
        S = HE->S(Vref,eref,z);
        cout << "P, T, S = " << P << ", " << T << ", " << S << "\n";
        double P0 = P;
        
        z[0] = 1.0;     // lambda = 1 (products)
        cout << "constant volume burn state\n";
        P = HE->P(Vref,eref,z);
        T = HE->T(Vref,eref,z);
        S = HE->S(Vref,eref,z);
        cout << "P, T, S = " << P << ", " << T << ", " << S << "\n";

        cout << "CJ state\n";
        WaveState CJ;
        HydroState state0(Vref,eref);
        Detonation *det = HE->detonation(state0,P0);
        if( det == NULL )
            cerr << Error("HE->detonation failed" ) << Exit;
        if( det->CJwave(RIGHT,CJ) )
            cerr << Error("det->CJwave failed" ) << Exit;
        T = products->T(CJ.V,CJ.e);
        cout << "V e P T = " << CJ.V << ", " << CJ.e 
                     << ", " << CJ.P << ", " << T << "\n";
        cout << "D, u, c = " << CJ.us << ", " << CJ.u
                     << ", " << products->c(CJ.V,CJ.e) << "\n";
        HydroState CJreactants;
        if( reactants->PT(CJ.P,T,CJreactants) )
            cerr << Error("reactants->PT failed" ) << Exit;
        V = CJreactants.V;
        e = CJreactants.e;
        z[0] = 0.0;
        if( HE->Rate(V,e,z,zdot) != 1 )
            cerr << Error("HE->Rate failed" ) << Exit;
        cout << "reactants at PT: V, e, rate = " << V << ", " << e
                                           << ", " << zdot[0] << "\n";    
        // test of exteos::var()
        double var;
        if( xeos->var("lambda",V,e,z,var) )
            cerr << Error("xeos->var(lambda) failed" ) << Exit;            
        cout << "xeox->var(lambda) = " << var << "\n";
        if( xeos->var("ddt_lambda",V,e,z,var) )
            cerr << Error("xeos->var(ddt_lambda) failed" ) << Exit;            
        cout << "xeox->var(ddt_lambda) = " << var << "\n";
        int status = xeos->var("timestep",V,e,z,var);
        cout << "xeox->var(timestep) = " << status << ", " << var << "\n";
        status = xeos->var("dummy",V,e,z,var);
        cout << "xeox->var(dummy) = " << status << ", " << var << "\n";
        //
        cout << "VN spike state\n";
        Hugoniot *hug = reactants->shock(state0);
        if( hug == NULL )
            cerr << Error("eos1->shock failed") << Exit;        
        WaveState VN;
        if( hug->u_s(CJ.us,RIGHT,VN) )
            cerr << Error("hug->u_s failed") << Exit;
        cout << "V e u = " << VN.V << ", " << VN.e << ", " << VN.u << "\n";
        cout << "P T = " << VN.P << ", " << reactants->T(VN.V,VN.e) << "\n";        
        if( HE->Rate(VN.V,VN.e,z,zdot) != 1 )
            cerr << Error("HE->Rate failed" ) << Exit;
        cout << "rate at VN = " << zdot[0] << "\n";
        if( hug->P(CJ.P,RIGHT,CJ) )
            cerr << Error("hug->P failed") << Exit;
        if( HE->Rate(CJ.V,CJ.e,z,zdot) != 1 )
            cerr << Error("HE->Rate failed" ) << Exit;        
        T = reactants->T(CJ.V,CJ.e);
        cout << "shock to Pcj: T, rate = " << T << ", " << HE->Rate(0.,T) << "\n";    
    }
    cout << "reactants->ref_count = " << reactants->RefCount() << "\n";
    cout << "products->ref_count = "  << products->RefCount()  << "\n";    
    deleteEOS(products);
    deleteEOS(reactants);
    cout << "eos->ref_count = " << eos->RefCount() << "\n";
    deleteEOS(eos);
    return 0;
}
