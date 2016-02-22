#include <Arg.h>
#include <LocalMath.h>
#include "ArrheniusHE.h"
#include <OneDFunction.h>

class partialCJ : public OneDFunction
{
public:
    ArrheniusHE &HE;
    EOS *reactants;
    EOS *products;
    HydroState state0;
    double P0;
    WaveState CJ;
    WaveState VN;
    double f(double V); // OneDFunction::F
    double z[1];
    int state(double lambda, WaveState &CJlambda);
    Detonation *det;
    Hugoniot *hug;
    partialCJ(ArrheniusHE &he);
    ~partialCJ();
};

partialCJ::partialCJ(ArrheniusHE &he) : HE(he)
{
    reactants = HE.reactants();
    products  = HE.products();
    double Vref = HE.V_ref; 
    double eref = HE.e_ref;
    state0.V = Vref;
    state0.e = eref;
    state0.u = 0.0;
    P0 = HE.P(Vref,eref);
    det = HE.detonation(state0,P0);
    if( det == NULL )
        cerr << Error("HE->detonation failed" ) << Exit;
    if( det->CJwave(RIGHT,CJ) )
        cerr << Error("det->CJwave failed" ) << Exit;
    hug = reactants->shock(state0);
    if( hug == NULL )
        cerr << Error("reactants->shock failed") << Exit;
    if( hug->u_s(CJ.us,RIGHT,VN) )
        cerr << Error("hug->u_s failed") << Exit;   
}
partialCJ::~partialCJ()
{
    delete det;
    delete hug;
}

double partialCJ::f(double V)
{
    double P = CJ.P +(CJ.V -V)/(CJ.V-VN.V) *(VN.P-CJ.P);
    double e = state0.e + 0.5*(P+P0)*(state0.V-V);
    return P - HE.P(V,e,z);
}

int partialCJ::state(double lambda, WaveState &CJlambda)
{
    if( lambda >= 1. )
    {
        CJlambda = CJ;
        z[0] = 1.;
        return 0;
    }
    if( lambda <= 0. )
    {
        CJlambda = VN;
        z[0] = 0.;
        return 0;
    }
    z[0] = lambda;
    double V = zero(CJ.V,VN.V);
    if( Status() )
    {
        cerr << Error("partialCJ::state: zero failed with status ")
             << ErrorStatus() << "\n";
        return 1;
    }
    double P  = CJ.P +(CJ.V -V)/(CJ.V-VN.V) *(VN.P-CJ.P);   
    CJlambda.V  = V;
    CJlambda.e  = state0.e + 0.5*(P+P0)*(state0.V-V);
    CJlambda.us = CJ.us;
    CJlambda.u  = CJ.us*(1.- V/state0.V);
    CJlambda.P  = P;
    return 0;   
}





#define NaN EOS::NaN
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "Test.data";
    const char *type = "ArrheniusHE";
    const char *name = "PBX9501";
    const char *units = NULL;
    double lambda = 0.8;

    EOS::Init();
    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        GetVar(lambda,lambda);
        ArgError;
    }
    DataBase db;
    if( db.Read(file) )
        cerr << Error("Read failed" ) << Exit;
    EOS *eos = FetchEOS(type,name,db);
    if( eos == NULL )
        cerr << Error("FetchEOS failed") << Exit;
    if( units && eos->ConvertUnits(units, db) )
        cerr << Error("ConvertUnits failed") << Exit;
    ArrheniusHE *HE = dynamic_cast<ArrheniusHE *>(eos);
    if( HE == NULL )
        cerr << Error("dynamic_cast failed") << Exit;
    partialCJ det(*HE);
    WaveState &CJ = det.CJ;
    WaveState &VN = det.VN;
    double D = CJ.us;
    cout << "D_CJ= "  << D << "\n";
    double T,c,ucD;
    cout << "VN spike state ("
         << det.reactants->Type() << "::" << det.reactants->Name()
         << ")\n";
    T = det.reactants->T(VN.V,VN.e);
    c = det.reactants->c(VN.V,VN.e);
    ucD = VN.u+c-D;
    cout << "V,e,u,P,T,c,u+c-D= "   << VN.V
                   << " " << VN.e 
                   << " " << VN.u 
                   << " " << VN.P 
                   << " " << T 
                   << " " << c 
                   << " " << ucD 
                   << "\n";
    WaveState VN1;
    if( det.state(lambda,VN1) )
        cerr << Exit;
    cout << "state ("
         << HE->Type() << "::" << HE->Name()
         << ") at lambda = " << lambda << "\n";
    T = HE->T(VN1.V,VN1.e,det.z);
    c = HE->c(VN1.V,VN1.e,det.z);
    ucD = VN1.u+c-D;
    cout << "V,e,u,P,T,c,u+c-D= "   << VN1.V
                   << " " << VN1.e 
                   << " " << VN1.u 
                   << " " << VN1.P 
                   << " " << T 
                   << " " << c
                   << " " << ucD 
                   << "\n";   
    cout << "CJ state ("
         << det.products->Type() << "::" << det.products->Name()
         << ")\n";
    T = det.products->T(CJ.V,CJ.e);
    c = det.products->c(CJ.V,CJ.e);
    ucD = CJ.u+c-D;
    cout << "V,e,u,P,T,c,u+c-D= " << CJ.V
                   << " " << CJ.e 
                   << " " << CJ.u 
                   << " " << CJ.P 
                   << " " << T 
                   << " " << c 
                   << " " << ucD 
                   << "\n";
    return 0;
}
