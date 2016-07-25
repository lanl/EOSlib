#include <Arg.h>
#include <LocalMath.h>
#include "StiffIdealHE.h"
//
#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "Test.data";
    const char *type = "StiffIdealHE";
    const char *name = "PBX9501";
    const char *units = NULL;

    EOS::Init();
    double V = NaN;
    double e = NaN;
    double P = 0.0;
    double T = 0.0;
    double lambda = 0.0;

    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        GetVar(V,V);
        GetVar(e,e);
        GetVar(P,P);
        GetVar(T,T);
        GetVar(lambda,lambda);
        
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
    StiffIdealHE *HE = dynamic_cast<StiffIdealHE *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    if( P > 0 && T > 0 )
    {
        HydroState PT;
        if( eos->PT(P,T,PT) )
            cerr << Error("PT failed") << Exit;
        cout << "P,T = " << P << ", " << T
             << ",  V,e = " << PT.V << ", " << PT.e << "\n";
        return 0;
    }
    if( !std::isnan(V) && !std::isnan(e) )
    {
        cout << "EOS\n"
             << "          T = " << eos->T(V,e)         << "\n"
             << "          P = " << eos->P(V,e)         << "\n"  
             << "          S = " << eos->S(V,e)         << "\n"
             << "          c = " << eos->c(V,e)         << "\n";
        double z[1] = {lambda};
        double t_induction = HE->Dt(V,e,z,0.5);
        cout << "half reaction time = " << t_induction << "\n";        
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

        cout << "\nCJ detonation state\n";
        WaveState CJ;
        HydroState state0(Vref,eref);
        Detonation *det = HE->detonation(state0,P0);
        if( det == NULL )
            cerr << Error("HE->detonation failed" ) << Exit;
        if( det->CJwave(RIGHT,CJ) )
            cerr << Error("det->CJwave failed" ) << Exit;
        z[0] = 1.0;     // lambda = 1 (products)
        T = HE->T(CJ.V,CJ.e,z);
        cout << "V e P T = " << CJ.V << ", " << CJ.e 
                     << ", " << CJ.P << ", " << T << "\n";
        cout << "D, u, c = " << CJ.us << ", " << CJ.u
                     << ", " << HE->c(CJ.V,CJ.e,z) << "\n";
        //
        cout << "VN spike state\n";
        Hugoniot *hug = HE->shock(state0);
        if( hug == NULL )
            cerr << Error("eos1->shock failed") << Exit;        
        WaveState VN;
        if( hug->u_s(CJ.us,RIGHT,VN) )
            cerr << Error("hug->u_s failed") << Exit;
        cout << "V e u = " << VN.V << ", " << VN.e << ", " << VN.u << "\n";
        cout << "P T = " << VN.P << ", " << HE->T(VN.V,VN.e) << "\n";        
        z[0] = 0.0;
        if( HE->Rate(VN.V,VN.e,z,zdot) != 1 )
            cerr << Error("HE->Rate failed" ) << Exit;
        cout << "rate at VN = " << zdot[0] << "\n";
        //
        if( hug->P(CJ.P,RIGHT,CJ) )
            cerr << Error("hug->P failed") << Exit;
        if( HE->Rate(CJ.V,CJ.e,z,zdot) != 1 )
            cerr << Error("HE->Rate failed" ) << Exit;        
        T = HE->T(CJ.V,CJ.e);
        cout << "\nshock to Pcj: T, rate = " << T << ", " << HE->Rate(0.,T) << "\n";    
        // test of exteos::var()
        double var;
        V = CJ.V;
        e = CJ.e;
        z[0] = 0.0;
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
        cout << "\nCJ deflagration state\n";
        Deflagration *def = HE->deflagration(state0,P0);
        if( def == NULL )
            cerr << Error("HE->deflagration failed" ) << Exit;
        if( def->CJwave(RIGHT,CJ) )
            cerr << Error("def->CJwave failed" ) << Exit;
        z[0] = 1.0;     // lambda = 1 (products)
        T = HE->T(CJ.V,CJ.e,z);
        cout << "V e P T = " << CJ.V << ", " << CJ.e 
                     << ", " << CJ.P << ", " << T << "\n";
        cout << "D, u, c = " << CJ.us << ", " << CJ.u
                     << ", " << HE->c(CJ.V,CJ.e,z) << "\n";        
    }
    deleteEOS(eos);
    return 0;
}
