#include <EOS.h>

#include <IdealGas.h>
#include <StiffenedGas.h>

void PrintStates(EOS &eos, HydroState &state, WaveState &wave)
{
    // Print initial state
    std::cout << "Ahead state\n";
    HydroStateLabel(std::cout) << " " << Center(P_form, "P")
                               << " " << Center(c_form, "c")
                               << " " << Center(T_form, "T")
                               << "\n";    
    std::cout << state << " " << P_form << eos.P(state)
                       << " " << c_form << eos.c(state)
                       << " " << T_form << eos.T(state)
                       << "\n";

    // Print shock state
    std::cout << "Shock state\n";
    WaveStateLabel(std::cout) << " " << Center(T_form, "T") << "\n";    
    std::cout << wave << " " << T_form << eos.T(wave) << "\n";
}

EOS *ideal_gas()
{
    // Ideal gas EOS can be called directly, unlike most others.
    // parameters corresponding to air at sea level in MKS units
    double gamma = 1.4;                 // adiabatic index of diatomic molecules
    double rho0  = 1.2;                 // kg/m^3
    double Cv    = 0.718e3;             // specific heat, (J/kg)/k at 300 K
    EOS *eos = new IdealGas(gamma,Cv);
    // ref state
    eos->V_ref = 1./rho0;
    eos->e_ref = 300.*Cv;
    return eos;
}

EOS *stiffened_gas()
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
    // parameters in units of hydro::std
    // matches initial shock locus, us = c0 + s*up
    param.Append("c0=2.6778");          // km/s
    param.Append("s=2.68946");          // dimensionless
    param.Append("P0=0.0001");          // GPa
    param.Append("Cv=1.05e-3");         // (MJ/kg)/K
    param.Append("e_ref=0.0");          //  MJ/kg
    param.Append("V_ref=0.52");         // cm^3/g
    // Note: no degree of freedom to set T0
    //       Complete EOS but thermal properties are not accurate

    StiffenedGas *eos = new StiffenedGas();
    if( eos->EOS::Initialize(param) ) 
    {
	    std::cout << "Error, Initialize failed\n"; 
    }
    return eos;
}
    
int main()
{ 
    EOS *eos;
    HydroState state;                   // (V,e,u)
    WaveState  wave;                    // (v,e,u,P,us)
    Hugoniot  *hugoniot;

    std::cout << "Ideal Gas: shock to V0/V=2\n";
    eos = ideal_gas();
    state.V = eos->V_ref;
    state.e = eos->e_ref;
    if( !(hugoniot=eos->shock(state)) )
    {
        std::cout << "Error, eos->shock(state) failed for Ideal Gas\n";
        return 1;
    }
    double V = 0.5*state.V;
    if( hugoniot->V(V, RIGHT, wave) )   // shock compression ratio V0/V = 2
    {
        // maximum compression ratio for ideal gas
        //      V0/V = (gamma+1)/(gamma-1) = 6 
        // so hugoniot will fail if V <= V0/(max compression ratio)
        std::cout << "Error, hugoniot->V failed for Ideal Gas\n";
        return 1;
    }  
    PrintStates(*eos,state,wave);
    // Deallocate storage
    delete hugoniot;
    deleteEOS(eos);

    std::cout << "\nStiffened Gas: shock to P/P0=1000\n";
    eos = stiffened_gas();
    state.V = eos->V_ref;
    state.e = eos->e_ref;
    if( !(hugoniot=eos->shock(state)) )
    {
        std::cout << "Error, eos->shock(state) failed for Stiffened Gas\n";
        return 1;
    }
    double P = 1000.*eos->P(state);
    if( hugoniot->P(P, RIGHT, wave) )   // shock pressure 1000*P0
    {
        // hugoniot will fail if P < P0
        std::cout << "Error, hugoniot->P failed for Stiffened Gas\n";
        return 1;
    }  
    PrintStates(*eos,state,wave);
    // Deallocate storage
    delete hugoniot;
    deleteEOS(eos);
}
