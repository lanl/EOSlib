#include <Arg.h>

#include "EOS.h"
#include "IdealGas.h"

#include <Riemann_gen.h>
#include "IdealGas_Riemann.h"
#include <iostream>

using namespace std;

int main(int, char **argv)
{
    ProgName(*argv);
    
    double gamma1 = 5./3.;
    double V1 = 1;
    double e1 = 1;
    double u1 = 0.0;
    
    double gamma2 = 7./5.;
    double V2 = 0.1;
    double e2 = 1;
    double u2 = 0.0;
    
    int solver = 0;                 // generic, specialized or both
    int count = 1;                  // for timing
    
    while(*++argv)
    {
        GetVar(gamma1,gamma1);
        GetVar(V1,V1);
        GetVar(e1,e1);
        GetVar(u1,u1);
        
        GetVar(gamma2,gamma2);
        GetVar(V2,V2);
        GetVar(e2,e2);
        GetVar(u2,u2);
        
        GetVarValue(generic,solver,-1);
        GetVarValue(specialized,solver,1);    
        GetVarValue(both,solver,0);
        
        GetVar(count, count);    
        
        ArgError;
    }
    cout.setf(ios::showpoint);

    
    HydroState left(V1,e1,u1);
    HydroState right(V2,e2,u2);
    
    IdealGas gas1(gamma1);
    IdealGas gas2(gamma2);
    
    WaveState wave_l, wave_r;

    gas1.Evaluate(left,  wave_l);
    gas2.Evaluate(right, wave_r);

    cout << "Left ideal gas, gamma = "  << gamma1 << "\n";
    cout << wave_l << "\n";
    cout << "Right ideal gas, gamma = " << gamma2 << "\n";
    cout << wave_r << "\n";

    //

    if( solver <= 0 )
    {
        RiemannSolver_generic RP(gas1, gas2);
            
        for(int i=count; i; i--)                    // for timing
            if( RP.Solve(left,right, wave_l, wave_r) )
                cerr << Error("RP.Solve failed") << Exit;
        
        cout << "Generic Riemann Solver\n";
        WaveStateLabel(cout) << "\n";
        cout << wave_l << "\n";
        cout << wave_r << "\n";
    }
    if( solver >= 0 )
    {
        RiemannSolver_IdealGas RP_IG(gas1, gas2);
    
        for(int i=count; i; i--)                    // for timing    
            if( RP_IG.Solve(left,right, wave_l, wave_r) )
                cerr << Error("RP_IG.Solve failed") << Exit;
        
        cout << "IdealGas Riemann Solver\n";
        WaveStateLabel(cout) << "\n";
        cout << wave_l << "\n";
        cout << wave_r << "\n";
    }        
    return 0;
}
