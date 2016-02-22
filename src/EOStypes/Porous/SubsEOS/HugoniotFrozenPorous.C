#include "Porous.h"

class FrozenPorousHugoniot : public Hugoniot
{
private:
    FrozenPorousHugoniot();                             // disallowed
    FrozenPorousHugoniot(const FrozenPorousHugoniot&);  // disallowed
    void operator=(const FrozenPorousHugoniot&);        // disallowed
    FrozenPorousHugoniot *operator &();                 // disallowed
    FrozenPorous &fp;
    Hugoniot *Ssolid;
public:
    FrozenPorousHugoniot(FrozenPorous &e, double pvac = EOS::NaN);
    ~FrozenPorousHugoniot();    
    int Initialize(const HydroState &state);

    int   P(double p,  int direction, WaveState &wave);
    int   V(double v,  int direction, WaveState &wave);
    int   u(double up, int direction, WaveState &wave);
    int u_s(double us, int direction, WaveState &wave);
};

FrozenPorousHugoniot::FrozenPorousHugoniot(FrozenPorous &e, double pvac)
            : Hugoniot(e, pvac), fp(e), Ssolid(NULL)
{
    // Null    
}

FrozenPorousHugoniot::~FrozenPorousHugoniot()
{
    delete Ssolid;
}

int FrozenPorousHugoniot::Initialize(const HydroState &state)
{
    delete Ssolid;    Ssolid = NULL;
    if( fp.solid == NULL )
        return 1;
    HydroState hs_solid(fp.phi_f*state.V, state.e - fp.B_f, state.u);
    Ssolid = fp.solid->shock(hs_solid);
    if( Ssolid == NULL )
        return 1;
    Ssolid->p_vac = p_vac/fp.phi_f;
    return 0;
}

int FrozenPorousHugoniot::P(double p, int direction, WaveState &wave)
{
    if( Ssolid == NULL )
        return -1;
    int status = Ssolid->P(p/fp.phi_f, direction, wave);
    wave.V /= fp.phi_f;
    wave.e += fp.B_f;
    wave.P *= fp.phi_f;
    return status;
}

int FrozenPorousHugoniot::V(double v, int direction, WaveState &wave)
{
    if( Ssolid == NULL )
        return -1;
    int status = Ssolid->V(fp.phi_f*v, direction, wave);
    wave.V /= fp.phi_f;
    wave.e += fp.B_f;
    wave.P *= fp.phi_f;
    return status;
}

int FrozenPorousHugoniot::u(double up, int direction, WaveState &wave)
{
    if( Ssolid == NULL )
        return -1;
    int status = Ssolid->u(up, direction, wave);
    wave.V /= fp.phi_f;
    wave.e += fp.B_f;
    wave.P *= fp.phi_f;
    return status;
}

int FrozenPorousHugoniot::u_s(double us, int direction, WaveState &wave)
{
    if( Ssolid == NULL )
        return -1;
    int status = Ssolid->u_s(us, direction, wave);
    wave.V /= fp.phi_f;
    wave.e += fp.B_f;
    wave.P *= fp.phi_f;
    return status;
}


Hugoniot *FrozenPorous::shock(const HydroState &state)
{
    Hugoniot *S = new FrozenPorousHugoniot(*this);
    if( S->Initialize(state) )
    {
        delete S;
        S = NULL;
        EOSerror->Log("FrozenPorous::shock", __FILE__, __LINE__, this,
                       "Initialize failed");
    }
    return S;    
}

