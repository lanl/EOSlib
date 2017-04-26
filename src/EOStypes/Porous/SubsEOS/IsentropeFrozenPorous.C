#include "Porous.h"

class FrozenPorousIsentrope : public Isentrope
{
private:
    FrozenPorousIsentrope();                             // disallowed
    FrozenPorousIsentrope(const FrozenPorousIsentrope&);  // disallowed
    void operator=(const FrozenPorousIsentrope&);        // disallowed
    FrozenPorousIsentrope *operator &();                 // disallowed
    FrozenPorous &fp;
    Isentrope *Ssolid;
public:
    FrozenPorousIsentrope(FrozenPorous &e, double pvac = EOS::NaN);
    ~FrozenPorousIsentrope();    
    int Initialize(const HydroState &state);

    int   P(double p,  int direction, WaveState &wave);
    int   V(double v,  int direction, WaveState &wave);
    int   u(double up, int direction, WaveState &wave);
    int u_s(double us, int direction, WaveState &wave);
    double u_escape(int direction);
    int    u_escape(int direction, WaveState &wave);
};

FrozenPorousIsentrope::FrozenPorousIsentrope(FrozenPorous &e, double pvac)
            : Isentrope(e, pvac), fp(e), Ssolid(NULL)
{
    // Null    
}

FrozenPorousIsentrope::~FrozenPorousIsentrope()
{
    delete Ssolid;
}

int FrozenPorousIsentrope::Initialize(const HydroState &state)
{
    delete Ssolid;    Ssolid = NULL;
    if( fp.solid == NULL )
        return 1;
    HydroState hs_solid(fp.phi_f*state.V, state.e - fp.B_f, state.u);
    Ssolid = fp.solid->isentrope(hs_solid);
    if( Ssolid == NULL )
        return 1;
    Ssolid->p_vac = p_vac/fp.phi_f;
    return 0;
}

int FrozenPorousIsentrope::P(double p, int direction, WaveState &wave)
{
    if( Ssolid == NULL )
        return -1;
    int status = Ssolid->P(p/fp.phi_f, direction, wave);
    wave.V /= fp.phi_f;
    wave.e += fp.B_f;
    wave.P *= fp.phi_f;
    return status;
}

int FrozenPorousIsentrope::V(double v, int direction, WaveState &wave)
{
    if( Ssolid == NULL )
        return -1;
    int status = Ssolid->V(fp.phi_f*v, direction, wave);
    wave.V /= fp.phi_f;
    wave.e += fp.B_f;
    wave.P *= fp.phi_f;
    return status;
}

int FrozenPorousIsentrope::u(double up, int direction, WaveState &wave)
{
    if( Ssolid == NULL )
        return -1;
    int status = Ssolid->u(up, direction, wave);
    wave.V /= fp.phi_f;
    wave.e += fp.B_f;
    wave.P *= fp.phi_f;
    return status;
}

int FrozenPorousIsentrope::u_s(double us, int direction, WaveState &wave)
{
    if( Ssolid == NULL )
        return -1;
    int status = Ssolid->u_s(us, direction, wave);
    wave.V /= fp.phi_f;
    wave.e += fp.B_f;
    wave.P *= fp.phi_f;
    return status;
}

double FrozenPorousIsentrope::u_escape(int direction)
{
    if( Ssolid == NULL )
        return EOS::NaN;
    return Ssolid->u_escape(direction);
}

int FrozenPorousIsentrope::u_escape(int direction, WaveState &wave)
{
    if( Ssolid == NULL )
        return 1;
    int status = Ssolid->u_escape(direction, wave);
    wave.V /= fp.phi_f;
    wave.e += fp.B_f;
    wave.P *= fp.phi_f;
    return status;    
}

Isentrope *FrozenPorous::isentrope(const HydroState &state)
{
    Isentrope *S = new FrozenPorousIsentrope(*this);
    if( S->Initialize(state) )
    {
        delete S;
        S = NULL;
        EOSerror->Log("FrozenPorous::isentrope", __FILE__, __LINE__, this,
                       "Initialize failed");
    }
    return S;    
}
