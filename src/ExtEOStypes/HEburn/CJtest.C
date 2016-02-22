#include <Arg.h>
#include <LocalMath.h>
#include "HEburn.h"
#include "EOS_VT.h"
//
#define NaN EOS::NaN
//
//
double TOL = 1e-10;    // first order
int  COUNT = 25;
//
int e_det(EOS *eos, double V0, double e0, double P0, double V, double &e)
{
// Given initial guess (V,e) for point on detonation locus
// refine e by Newton iteration
//  f(e) = e-e0 - 0.5*(P+P0)*(V0-V)
//  df/de = 1 - 0.5*(Gamma/V)*(V0-V)
//  e = e - f(e)/(df/de)
    int count = COUNT;
    while( count-- )
    {
        double P = eos->P(V,e);
        double PdV =  0.5*(P+P0)*(V0-V);
        double f = e-e0 - PdV;
        if( abs(f) < TOL*PdV )
            return 0;
        double Gamma = eos->Gamma(V,e);
        double dfde = 1. - 0.5*(Gamma/V)*(V0-V);
        e -= f/dfde;
    }
    return 1;
}
//
//  Test of algorithm to calculate CJ state
//
int main(int, char **argv)
{
    ProgName(*argv);
    const char *file = "Test.data";
    const char *type = "HEburn";
    const char *name = "PBX9501.sam";
    const char *units = NULL;

    EOS::Init();
    double V0 = NaN;
    double e0 = NaN;
    double P0 = NaN;

    // debug
    //file = "HE.data";
    //name = "PBX9501.davis";

    while(*++argv)
    {
        GetVar(file,file);
        GetVar(files,file);
        GetVar(name,name);
        GetVar(type,type);
        GetVar(units,units);
        
        GetVar(V0,V0);
        GetVar(e0,e0);
        GetVar(P0,P0);
        
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
    EOS *reactants = HE->Reactants();
    EOS *products  = HE->Products();
    if( isnan(V0) ) V0 = eos->V_ref;
    if( isnan(e0) ) e0 = eos->e_ref;
    if( isnan(P0) ) P0 = reactants->P(V0,e0);
    cerr << "debug: V0,e0,P0 = " << V0 << " " << e0 << " " << P0 << "\n";
    //
    // constant volume burn
    double Tcvb = products->T(V0,e0);
    double Pcvb = products->P(V0,e0);
    cerr << "debug: Tcb, Pcb = " << Tcvb << " " << Pcvb << "\n";
    
    //HydroState CVB(V0,e0);
    //Isotherm *isotherm = products(CVB);
    //EOS_VT *products_VT = products->eosVT();

    // bounds
    double V1, e1, P1, f1;
    double V2, e2, P2, f2;
    double c2, rhoD2, rhoc2;

    // initial guess
    V1 = 0.8 * V0;
    //e1 = products_VT->e(V1,Tcvb);
    e1 = e0 + Pcvb*(V0-V1);

    if( e_det(products,V0,e0,P0, V1,e1) )
        cerr << Error( "e_det failed at V1,e1 = " )
             << V1 << " " << e1 << Exit;

    P1 = products->P(V1,e1);
    c2 = products->c2(V1,e1);
    rhoD2 = (P1-P0)/(V0-V1);
    rhoc2 = c2/(V1*V1);
    f1 =  rhoD2 - rhoc2;

    // find other bound
    int count = COUNT;
    if( f1 > 0. )
    {
        double Vmin = 0.0;
        while(count--)
        {
            V2 = Vmin + 0.9*(V1-Vmin);
            e2 = e1;
            if( e_det(products,V0,e0,P0, V2,e2) )
            {
                Vmin = V2;
                continue;
            }
            V2 = 0.9*V1;
            e2 = e1;
            if( e_det(products,V0,e0,P0, V2,e2) )
                cerr << Error( "e_det failed at V2,e2 = " )
                     << V2 << " " << e2 << Exit;
            P2 = products->P(V2,e2);
            c2 = products->c2(V2,e2);
            rhoD2 = (P2-P0)/(V0-V2);
            rhoc2 = c2/(V2*V2);
            f2 =  rhoD2 - rhoc2;
            if( f2 <= 0. )
                break;
            else
            {
                V1 = V2;
                e1 = e2;
                f1 = f2;
                P1 = P2;
            }
        }
    }
    else if( f1 < 0. )
    {
        V2 = V1;
        e2 = e1;
        f2 = f1;
        P2 = P1;
        while(count--)
        {
            V1 = 0.5*(V2+V0);
            e1 = e2;
            if( e_det(products,V0,e0,P0, V1,e1) )
                cerr << Error( "e_det failed at V1,e1 = " )
                     << V1 << " " << e1 << Exit;
            P1 = products->P(V1,e1);
            c2 = products->c2(V1,e1);
            rhoD2 = (P2-P0)/(V0-V2);
            rhoc2 = c2/(V2*V2);
            f1 =  rhoD2 - rhoc2;
            if( f1 >= 0. )
                break;
            else
            {
                V2 = V1;
                e2 = e1;
                f2 = f1;
                P2 = P1;
            }
        }
    }
    else
    { // lucky guess
        V2 = V1;
        e2 = e1;
        P2 = P1;
        f2 = f1;
    }
    if( count <= 0 )
        cerr << Error( "bound failed\n " ) << Exit;
    // V2 < V1, P2 > P1, f2 < 0 < f1

    cerr << "bounds\n";
    cerr << "V2 e2 P2 f2 = " << V2 << " " << e2
         << " " << P2 << " " << f2 << "\n";
    cerr << "V1 e1 P1 f1 = " << V1 << " " << e1
         << " " << P1 << " " << f1 << "\n";

    // bisection
    double V,e, P,f;
    count = 2*COUNT;
    while(count--)
    {
        V = 0.5*(V1+V2);
        e = 0.5*(e1+e2);
        if( e_det(products,V0,e0,P0, V,e) )
            cerr << Error( "e_det failed at V,e = " )
                 << V << " " << e1<< Exit;
        P = products->P(V,e);
        c2 = products->c2(V,e);
        rhoD2 = (P-P0)/(V0-V);
        rhoc2 = c2/(V*V);
        f =  rhoD2 - rhoc2;
        if( abs(f) < TOL*rhoD2 )
            break;
        if( f < 0. )
        {
            V2 = V;
            e2 = e;
            P2 = P;
            f2 = f;
        }
        else
        {
            V1 = V;
            e1 = e;
            P1 = P;
            f1 = f;
        }
    }
    if( count <= 0 )
        cerr << Error( "bisection failed\n " ) << Exit;
    cerr << "count = " << 2*COUNT - count << "\n";

    cerr << "V e P f = " << V << " " << e
         << " " << P << " " << f << "\n";
    double Dcj = sqrt(rhoD2)*V0;
    cerr << "Dcj = " << Dcj << "\n";

    //deleteEOS_VT(products_VT);
    deleteEOS(eos);
    return 0;
}
