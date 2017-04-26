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

// Comment out calc line gives seg fault on next FetchEOS
/***/
std::cout << "DEBUG: before Calc\n";
Calc calc;
const char *line = "exp(1)";
double ans=0.;
//calc.parse(line,ans);
std::cout << "DEBUG: calc parse ans " << ans << "\n";
/***/
    EOS *eos;
// comment out next line in addition to Calc lines works
/***/
    eos = FetchEOS("ArrheniusHE::HMX",db);
std::cout << "DEBUG: after Fetch ExtEOS\n";
/***/
    // Fetch EOS
    eos = FetchEOS("IdealGas::7/5",db);
std::cout << "DEBUG: after FetchEOS\n";
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
    HydroStateLabel(std::cout) << " " << Center(P_form, "P")
                               << " " << Center(c_form, "c")
                               << "\n";    
    HydroStateLabel(std::cout,u) << "\n";
    std::cout << state << " " << P_form << eos->P(state)
                       << " " << c_form << eos->c(state)
                       << "\n";
    // Initialize Hugoniot
    Hugoniot *hugoniot = eos->shock(state);
    
    // Compute point on Hugoniot locus with specified V
    WaveState wave;
    double V = 0.5*state.V;
    if( hugoniot->V(V, RIGHT, wave) )       // shock with compression ratio of 2
    {
        // maximum compression ratio for ideal gas
        //      V0/V = (gamma+1)/(gamma-1) = 6 @ gamma = 1.4
        // so hugoniot will fail if V <= 1/6
        std::cout << "Error, hugoniot->V failed\n";
        return 1;
    }
    // Print shock state
    std::cout << "\nShock wave state at V0/V=2\n";
    WaveStateLabel(std::cout)   << "\n";
    WaveStateLabel(std::cout,u) << "\n";
    std::cout << wave << "\n";

    // Compute point on Hugoniot locus with specified P
    double P = 100.*eos->P(state);
    if( hugoniot->P(P, RIGHT, wave) )       // shock with P = 100*P0
    {
        std::cout << "Error, hugoniot->P failed\n";
        return 1;
    }
    // Print shock state
    std::cout << "\nShock wave state at P/P0=100\n";
    WaveStateLabel(std::cout)   << "\n";
    WaveStateLabel(std::cout,u) << "\n";
    std::cout << wave << "\n";

    // Deallocate storage
    delete hugoniot;

    // convert to hydro::std units
    if( eos->ConvertUnits("hydro::std", db) )
    {
        std::cout << "\neos->ConvertUnits failed\n";
        return 1;
    }
    state.V = eos->V_ref;
    state.e = eos->e_ref;
    hugoniot = eos->shock(state);
    P = 100.*eos->P(state);
    if( hugoniot->P(P, RIGHT, wave) )       // shock with P = 100*P0
    {
        std::cout << "Error, hugoniot->P failed\n";
        return 1;
    }
    // Print shock state
    std::cout << "\nShock wave state in 'hydro::std' units\n";
    WaveStateLabel(std::cout)   << "\n";
    const Units &u1 = *eos->UseUnits();
    WaveStateLabel(std::cout,u1) << "\n";
    std::cout << wave << "\n";

    // Deallocate storage
    delete hugoniot;
    deleteEOS(eos);

std::cout << "Before fetch ArrheniusHE\n";
    eos = FetchEOS("ArrheniusHE::HMX",db);
    eos->Print(std::cout) << "\n";
    eos = FetchEOS("HEburn::PBX9502.davis",db);
    
}
