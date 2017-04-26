#include "Porous.h"

class FrozenPorousIsotherm : public Isotherm
{
private:
    FrozenPorousIsotherm();                             // disallowed
    FrozenPorousIsotherm(const FrozenPorousIsotherm&);  // disallowed
    void operator=(const FrozenPorousIsotherm&);        // disallowed
    FrozenPorousIsotherm *operator &();                 // disallowed
    FrozenPorous &fp;
    Isotherm *Tsolid;
public:
    FrozenPorousIsotherm(FrozenPorous &e, double pvac = EOS::NaN);
    ~FrozenPorousIsotherm();    
    int Initialize(const HydroState &state);

    int P(double p, ThermalState &state);
    int V(double v, ThermalState &state);
};

FrozenPorousIsotherm::FrozenPorousIsotherm(FrozenPorous &e, double pvac)
            : Isotherm(e, pvac), fp(e), Tsolid(NULL)
{
    // Null    
}

FrozenPorousIsotherm::~FrozenPorousIsotherm()
{
    delete Tsolid;
}

int FrozenPorousIsotherm::Initialize(const HydroState &state)
{
    delete Tsolid;
    Tsolid = NULL;
    if( fp.solid == NULL )
        return 1;
    HydroState hs_solid(fp.phi_f*state.V, state.e - fp.B_f, state.u);
    Tsolid = fp.solid->isotherm(hs_solid);
    if( Tsolid == NULL )
        return 1;
    Tsolid->p_vac = p_vac/fp.phi_f;
    return 0;
}

int FrozenPorousIsotherm::P(double p, ThermalState &state)
{
    if( Tsolid == NULL )
        return -1;
    int status = Tsolid->P(p/fp.phi_f, state);
    state.V /= fp.phi_f;
    state.e += fp.B_f;
    state.P *= fp.phi_f;
    return status;
}

int FrozenPorousIsotherm::V(double v, ThermalState &state)
{
    if( Tsolid == NULL )
        return -1;
    int status = Tsolid->V(fp.phi_f*v, state);
    state.V /= fp.phi_f;
    state.e += fp.B_f;
    state.P *= fp.phi_f;
    return status;
}



Isotherm *FrozenPorous::isotherm(const HydroState &state)
{
    Isotherm *T = new FrozenPorousIsotherm(*this);
    if( T->Initialize(state) )
    {
        delete T;
        T = NULL;
        EOSerror->Log("FrozenPorous::isotherm", __FILE__, __LINE__, this,
                       "Initialize failed");
    }
    return T;    
}
