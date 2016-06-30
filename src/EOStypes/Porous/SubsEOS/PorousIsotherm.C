#include <LocalMath.h>

#include <EOS.h>
#include "Porous.h"

class IsothermEqPorous : public Isotherm, private OneDFunction
{
    enum Isotherm_type
    {
        pressure=0, specificV=1
    };
private:
    EqPorous &peos;
    Isotherm *Isolid;                               // isotherm for pure solid
    double f(double);                               // OneDFunction::f
    Isotherm_type type;
    double P_f, V_f;
    double phi, e[3];
    ThermalState state_s;
    
    IsothermEqPorous ();                            // disallowed
    void operator=(const IsothermEqPorous&);        // disallowed
    IsothermEqPorous(const IsothermEqPorous&);      // disallowed
    IsothermEqPorous *operator &();                 // disallowed
public:
    IsothermEqPorous(EqPorous &e, double pvac = EOS::NaN);
    ~IsothermEqPorous();
    
    int Initialize(const HydroState &state);
    
    int V(double v, ThermalState &wave);
    int P(double p, ThermalState &wave);
};

Isotherm *EqPorous::isotherm(const HydroState &state)
{
    Isotherm *T = new IsothermEqPorous(*this);
    if( T->Initialize(state) )
    {
        delete T;
        T = NULL;
        EOSerror->Log("EqPorous::isotherm", __FILE__, __LINE__, this,
               "Initialize failed");
    }
    return T;
}

//

IsothermEqPorous::IsothermEqPorous(EqPorous &e, double pvac)
                        : Isotherm(e, pvac), peos(e), Isolid(NULL)
{
    // Null
}

IsothermEqPorous::~IsothermEqPorous()
{
    delete Isolid;
}

int IsothermEqPorous::Initialize(const HydroState &state)
{
    peos.Evaluate(state,state0);
    HydroState s_state(peos.phi_f*state0.V, state0.e-peos.B_f);
    delete Isolid;
    Isolid = peos.solid->isotherm(s_state);
    return Isolid == NULL;
}

double IsothermEqPorous::f(double v)
{
    if( Isolid->V(v,state_s) < 0 )
    {
        SetError();
        return EOS::NaN;
    }
    
    switch (type)
    {
    case pressure:
        phi = P_f/state_s.P;
        break;
    case specificV:
        phi = state_s.V/V_f;
        break;
    }
    if( peos.phi_eq->energy(phi,e) )
    {
        SetError();
        return EOS::NaN;
    }
    return state_s.P*state_s.V - phi*e[1];
}

int IsothermEqPorous::V(double v, ThermalState &state)
{
    type = specificV; V_f = v;
    if( Isolid->V(v*peos.phi_0(),state_s) )
    {
        eos->ErrorHandler()->Log("IsothermEqPorous::V",__FILE__,__LINE__,
                eos, "Isolid failed at phi_1\n");
        return -1;
    }
    double Vmin = state_s.V;
    double fmin = f(Vmin);
    if( std::isnan(fmin) || fmin < 0 )
    {
        eos->ErrorHandler()->Log("IsothermEqPorous::V",__FILE__,__LINE__,
                eos, "fmin = NaN or < 0\n");
        return -1;
    }
    if( Isolid->V(v*peos.phi_1(),state_s) < 0 )
    {
        eos->ErrorHandler()->Log("IsothermEqPorous::V",__FILE__,__LINE__,
                eos, "Isolid failed at phi_0\n");
        return -1;
    }
    double Vmax = state_s.V;
    double fmax = f(Vmax);
    if( std::isnan(fmax) )
    {
        eos->ErrorHandler()->Log("IsothermEqPorous::V",
                 __FILE__,__LINE__, eos, "fmax = NaN\n");
        return -1;
    }
    if( fmax < 0 )
    {
        double vs = zero(Vmin,fmin, Vmax,fmax);
        if( Status() )
        {
            eos->ErrorHandler()->Log("IsothermEqPorous::V",__FILE__,__LINE__,
                    eos, "zero failed with status: %s\n", ErrorStatus());
            return -1;
        }
        (void) f(vs);
    }
    state.V = state_s.V/phi;
    state.e = state_s.e + e[0];
    state.P = phi*state_s.P;
    state.T = state_s.T;
    return 0;
}

int IsothermEqPorous::P(double p, ThermalState &state)
{
    type = pressure; P_f = p;

    if( Isolid->P(p/peos.phi_1(),state_s) < 0 )
    {
        eos->ErrorHandler()->Log("IsothermEqPorous::P",__FILE__,__LINE__,
                eos, "Isolid failed at phi_1\n");
        return -1;
    }
    double Vmax = state_s.V;
    double fmax = f(Vmax);
    if( fmax < 0 )
    {
        if( Isolid->P(p/peos.phi_0(),state_s) < 0 )
        {
            eos->ErrorHandler()->Log("IsothermEqPorous::P",__FILE__,__LINE__,
                    eos, "Isolid failed at phi_0\n");
            return -1;
        }
        double Vmin = state_s.V;
        double fmin = f(Vmin);
        double vs = zero(Vmin,fmin, Vmax,fmax);
        if( Status() )
        {
            eos->ErrorHandler()->Log("IsothermEqPorous::P",__FILE__,__LINE__,
                    eos, "zero failed with status: %s\n", ErrorStatus());
            return -1;
        }
        (void) f(vs);
    }
    state.V = state_s.V/phi;
    state.e = state_s.e + e[0];
    state.P = phi*state_s.P;
    state.T = state_s.T;
    return 0;
}
