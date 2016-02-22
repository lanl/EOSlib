#include "Porous.h"

FrozenPorous::FrozenPorous(EqPorous &porous, double phi1, double B1,
              const char *fpname) : EOS("FrozenPorous"), phi_f(phi1), B_f(B1)
{
    Copy((EOS &)porous,0);
    solid  = porous.solid;
    phi_eq = porous.phi_eq;
    if( fpname )
        name = Strdup(fpname);
    else
        name = Cat(porous.name,".frozen");
    if( solid == NULL || phi_eq == NULL )
    {
        EOSstatus = EOSlib::bad;
        EOSerror->Abort("FrozenPorous::FrozenPorous", __FILE__, __LINE__, this,
                         "porous is not well defined");
    }
    else if( isnan(B_f) )
        B_f = phi_eq->energy(phi_f);
}

void FrozenPorous::Reset(double phi1, double B1)
{
    phi_f = phi1;
    B_f = isnan(B1) ? phi_eq->energy(phi_f) : B1;
}

FrozenPorous::~FrozenPorous()
{
    deleteEOS(solid);
    deletePhiEq(phi_eq);
}



double FrozenPorous::P(double V, double e)
{
    return phi_f*solid->P(phi_f*V,e-B_f);
}

double FrozenPorous::T(double V, double e)
{
    return solid->T(phi_f*V,e-B_f);
}

double FrozenPorous::S(double V, double e)
{
    return solid->S(phi_f*V,e-B_f);
}

    
double FrozenPorous::c2(double V, double e)
{
    return solid->c2(phi_f*V,e-B_f);
}

double FrozenPorous::Gamma(double V, double e)
{
    return solid->Gamma(phi_f*V,e-B_f);
}

double FrozenPorous::CV(double V, double e)
{
    return solid->CV(phi_f*V,e-B_f);
}

int FrozenPorous::NotInDomain(double V, double e)
{
    return solid ? solid->NotInDomain(phi_f*V,e-B_f) : 1;
}
