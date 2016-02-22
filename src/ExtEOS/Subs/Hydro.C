#include "Hydro.h"

Hydro::Hydro(EOS &eos) : ExtEOS("Hydro")
{
    hydro = eos.Duplicate();
    if( hydro == NULL )
    {
        EOSstatus = EOSlib::abort;
        return;
    }   
    EOS::Copy(*hydro,0);
    char *fullname = Cat(hydro->Type(),"::",hydro->Name());
    Rename(fullname);
    delete [] fullname;
    EOSstatus = EOSlib::good;
}

Hydro::~Hydro()
{
    deleteEOS(hydro);
}

ExtEOS *Hydro::Copy()
{
    return static_cast<ExtEOS*>(EOSbase::Duplicate());
}
ExtEOS *Hydro::Copy(const double *)
{
    return static_cast<ExtEOS*>(EOSbase::Duplicate());
}
EOS *Hydro::CopyEOS(const double *)
{
    return hydro ? hydro->Duplicate() : NULL;
}

double *Hydro::z_f(double, double)
{
    return NULL;
}

void Hydro::PrintParams(ostream &out)
{
    ExtEOS::PrintParams(out);
    if( hydro )
    {
         const char *tp = hydro->Type();
         const char *nm = hydro->Name();
         out << "\tEOS = \""
             << (tp ? tp : "NULL") << "::"
             << (nm ? nm : "NULL") << "\"\n";
    }
}
ostream &Hydro::PrintComponents(ostream &out)
{
    if( hydro )
        hydro->PrintAll(out);
    return out;
}

int Hydro::ConvertParams(Convert &convert)
{
    if( ExtEOS::ConvertParams(convert) )
        return 1;
    return hydro ? hydro->ConvertUnits(convert) : 1;
}

// EOS functions
double Hydro::P(double V, double e)
{
    return hydro ? hydro->P(V,e) : NaN;
}
double Hydro::T(double V, double e)
{
    return hydro ? hydro->T(V,e) : NaN;
}
double Hydro::S(double V, double e)
{
    return hydro ? hydro->S(V,e) : NaN;
}
double Hydro::c2(double V, double e)
{
    return hydro ? hydro->c2(V,e) : NaN;
}
double Hydro::Gamma(double V, double e)
{
    return hydro ? hydro->Gamma(V,e) : NaN;
}
double Hydro::CV(double V, double e)
{
    return hydro ? hydro->CV(V,e) : NaN;
}
double Hydro::CP(double V, double e)
{
    return hydro ? hydro->CP(V,e) : NaN;
}
double Hydro::KT(double V, double e)
{
    return hydro ? hydro->KT(V,e) : NaN;
}
double Hydro::beta(double V, double e)
{
    return hydro ? hydro->beta(V,e) : NaN;
}
double Hydro::FD(double V, double e)
{
    return hydro ? hydro->FD(V,e) : NaN;
}
double Hydro::e_PV(double p, double V)
{
    return hydro ? hydro->e_PV(p,V) : NaN;
}
int Hydro::NotInDomain(double V, double e)
{
    return hydro ? hydro->NotInDomain(V,e) : 1;
}
//
Isentrope *Hydro::isentrope(const HydroState &state)
{
    return hydro ? hydro->isentrope(state) : NULL;
}
Isentrope *Hydro::isentrope(const HydroState &state, const double*)
{
    return hydro ? hydro->isentrope(state) : NULL;
}
Hugoniot *Hydro::shock(const HydroState &state)
{
    return hydro ? hydro->shock(state) : NULL;
}
Hugoniot *Hydro::shock(const HydroState &state, const double*)
{
    return hydro ? hydro->shock(state) : NULL;
}
Isotherm *Hydro::isotherm(const HydroState &state)
{
    return hydro ? hydro->isotherm(state) : NULL;
}
Isotherm *Hydro::isotherm(const HydroState &state, const double*)
{
    return hydro ? hydro->isotherm(state) : NULL;
}
Detonation *Hydro::detonation(const HydroState &state, double p0)
{
    return hydro ? hydro->detonation(state, p0) : NULL;
}
int Hydro::PT(double P, double T, HydroState &state)
{
    return hydro ? hydro->PT(P,T,state) : 1;
}
//
// ExtEOS functions
// 
double Hydro::P(double V, double e, const double *)
{
    return hydro ? hydro->P(V,e) : NaN;
}
double Hydro::T(double V, double e, const double *)
{
    return hydro ? hydro->T(V,e) : NaN;
}
double Hydro::S(double V, double e, const double *)
{
    return hydro ? hydro->S(V,e) : NaN;
}
double Hydro::c2(double V, double e, const double *)
{
    return hydro ? hydro->c2(V,e) : NaN;
}
double Hydro::Gamma(double V, double e, const double *)
{
    return hydro ? hydro->Gamma(V,e) : NaN;
}
double Hydro::CV(double V, double e, const double *)
{
    return hydro ? hydro->CV(V,e) : NaN;
}
double Hydro::CP(double V, double e, const double *)
{
    return hydro ? hydro->CP(V,e) : NaN;
}
double Hydro::KT(double V, double e, const double *)
{
    return hydro ? hydro->KT(V,e) : NaN;
}
double Hydro::beta(double V, double e, const double *)
{
    return hydro ? hydro->beta(V,e) : NaN;
}
double Hydro::FD(double V, double e, const double *)
{
    return hydro ? hydro->FD(V,e) : NaN;
}
double Hydro::e_PV(double p, double V, const double *)
{
    return hydro ? hydro->e_PV(p,V) : NaN;
}
int Hydro::NotInDomain(double V, double e, const double *)
{
    return hydro ? hydro->NotInDomain(V,e) : 1;
}
//
//
//
int Hydro::Rate(double, double, const double *, double *)
{
    return 0;
}
int Hydro::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0;
    return 0;   // no constraint
}
int Hydro::Integrate(double V, double e, double *z, double dt)
{
    return 0;
}
int Hydro::Equilibrate(double V, double e, double *z)
{
    return 0;
}
