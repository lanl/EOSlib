#include <EOS.h>
#include <IdealGas.h>

#include <StiffenedGas.h>

int common(EOS *eos)
{
    // Define initial hydro state
    HydroState state(1.0,1.0,0.0);  // V1 = 1, e1 = 1, u1 = 0

    // Print initial state
    std::cout << "Ahead state\n";
    HydroStateLabel(std::cout) << " " << Center(P_form, "P")
                               << " " << Center(c_form, "c")
                               << "\n";    
    std::cout << state << " " << P_form << eos->P(state)
                       << " " << c_form << eos->c(state)
                       << "\n";
    
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
    std::cout << "Shock wave state\n";
    WaveStateLabel(std::cout) << "\n";    
    std::cout << wave << "\n";

    // Deallocate storage
    delete hugoniot;
    deleteEOS(eos);

    return 0;
}

EOS* ideal_gas()
{
    // Define EOS
    // Ideal gas EOS can be called directly, unlike most others.
    EOS *eos = new IdealGas(1.4);   // gamma = 1.4
    return eos;
}

EOS* stiffened_gas()
{
    // Define EOS
    // 
    // Most EOS derived classes have too many parameters to be effectively set
    // as arguments to the constructor. If you really want to set these 
    // directly, instead of through the database, then you can do it like this:
    Parameters param; 
    param.base = Strdup("EOS");
    param.type = Strdup("StiffenedGas");
    param.name = Strdup("HMX");
    param.Append("c0","2.6778");
    param.Append("s","2.68946");
    param.Append("P0=0.0001");
    param.Append("Cv=1.05e-3");
    param.Append("e_ref=0.0");
    param.Append("V_ref=0.52");

    std::cout << "Got this far\n";
    std::cout.flush();

    StiffenedGas *eos;
    if( eos->EOS::Initialize(param) ) 
    {
	    std::cout << "Error, Initialize failed\n"; 
    }
    std::cout << "Got this far\n";
    std::cout.flush();
    return eos;
}
    
int main()
{ 
    std::cout << "Ideal Gas: \n";
    EOS* eos = ideal_gas();
    common(eos);
    std::cout << "Stiffened Gas: \n";
    EOS* eos2 = stiffened_gas();
    common(eos);

}


