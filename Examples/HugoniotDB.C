#include <stdlib.h>
#include <EOS.h>

int main()
{
    // initialize EOS DataBase
    DataBase db;
    const char *file;
    if( !(file=getenv("EOSdata")) )
    {
       std::cout  << "no environment variable `EOSdata'\n";
       return 1;
    }
    if( db.Read(file) )
    {
        std::cout << "db.Read(file) failed";
        return 1;
    }

    // Fetch EOS
    EOS *eos = FetchEOS("IdealGas::7/5",db);
    if( eos==NULL )
    {
        std::cout << "Fetch `IdealGas::7/5' failed\n";
        return 1;
    }
    std::cout << "EOS type " << eos->Type()
              <<         " name " << eos->Name() << "\n";
    const Units &u = *eos->UseUnits();
    std::cout << "units " << u.Name() << "\n";

    // Define initial hydro state (eos ref state)
    HydroState state(eos->V_ref,eos->e_ref,0.0);
    // Print initial state
    std::cout << "\nAhead hydro state\n";
    HydroStateLabel(std::cout)   << "\n";
    HydroStateLabel(std::cout,u) << "\n";
    std::cout << state << "\n";

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
    std::cout << "\nShock wave state\n";
    WaveStateLabel(std::cout)   << "\n";
    WaveStateLabel(std::cout,u) << "\n";
    std::cout << wave << "\n";

    // Deallocate storage
    delete hugoniot;
    deleteEOS(eos);

    return 0;
}
