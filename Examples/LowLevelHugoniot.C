#include <EOS.h>
#include <IdealGas.h>

int main()
{
    // Define EOS
    EOS *eos = new IdealGas(1.4);   // gamma = 1.4

    // Define initial hydro state
    HydroState state(1.0,1.0,0.0);  // V1 = 1, e1 = 1, u1 = 0
    // Print initial state
    std::cout << "Ahead state: V, e, u\n";
    std::cout << state << eos->P(state) << "\n";

    // Initialize Hugoniot
    Hugoniot *hugoniot = eos->shock(state);
    
    // Compute point on Hugoniot locus with specified V
    WaveState wave;
    if( hugoniot->V(0.5, RIGHT, wave) )
    {
        // maximum compression ratio for ideal gas
        //      V0/V = (gamma+1)/(gamma-1) = 6 @ gamma = 1.4
        // so hugoniot will fail if V <= 1/6
        std::cout << "Error, hugoniot failed\n";
        return 1;
    }
    // Print shock state
    std::cout << "Shock wave state: V, e, u, P, us\n";
    std::cout << wave << "\n";

    // Deallocate storage
    delete hugoniot;
    deleteEOS(eos);

    return 0;
}
