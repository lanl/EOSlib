#include "EqExtEOS.h"

extern "C" { // DataBase functions
  char *EOS_EqExtEOS_Init()
  {
      return NULL;
  }
  char *EOS_EqExtEOS_Info()
  {
      char *msg = Strdup("EqExtEOS derived from ExtEOS");
      return msg;
  }
  void *EOS_EqExtEOS_Constructor()
  {
      EOS *eos = new EqExtEOS;
      return static_cast<void*>(eos);
  }
}


EqExtEOS::EqExtEOS() : ExtEOS("EqExtEOS"), exteos(NULL)
{
    frozen = 0;
}

EqExtEOS::EqExtEOS(ExtEOS &e_eos) : ExtEOS(e_eos)
{
    frozen = 0;
    exteos = e_eos.Duplicate();
    delete [] type;
    type = Strdup("EqExtEOS");
}

EqExtEOS::EqExtEOS(ExtEOS &e_eos, const double *z) : ExtEOS(e_eos)
{
    frozen = 0;
    exteos = e_eos.Copy(z);
    delete [] type;
    type = Strdup("EqExtEOS");
}

EqExtEOS::~EqExtEOS()
{
    z_ref = NULL;
    deleteExtEOS(exteos);
}

ExtEOS *EqExtEOS::Copy()
{
    return exteos ? new EqExtEOS(*exteos) : NULL;
}

int EqExtEOS::PreInit(Calc &calc)
{
    char *eos_name = NULL;
    calc.Variable( "ExtEOS", eos_name );
    return 0;
}
#define FUNC "EqExtEOS::PostInit",__FILE__,__LINE__,this
int EqExtEOS::PostInit(Calc &calc, DataBase *db)
{
    const char *eos_name;
    calc.Fetch("ExtEOS",eos_name);
    if( eos_name == NULL )
    {
        EOSerror->Log(FUNC, "ExtEOS not specified\n" );
        return 1;
    }
    exteos = FetchExtEOS(eos_name,*db);
    if( exteos == NULL )
    {
        EOSerror->Log(FUNC, "FetchExtEOS failed for %s\n", eos_name );
        return 1;
    }
    if( units && exteos->ConvertUnits(*units) )
    {
        EOSerror->Log(FUNC, "exteos->ConvertUnits(%s::%s) failed\n",
                      units->Type(),units->Name() );
        return 1;
    }
    z_ref = exteos->Zref();
    if( std::isnan(V_ref) )
        V_ref = exteos->V_ref;
    if( std::isnan(e_ref) )
        e_ref = exteos->e_ref;
	return 0;
}

void EqExtEOS::PrintParams(ostream &out)
{
    ExtEOS::PrintParams(out);
    if( exteos )
    {
         const char *tp = exteos->Type();
         const char *nm = exteos->Name();
         out << "\tExtEOS = \""
             << (tp ? tp : "NULL") << "::"
             << (nm ? nm : "NULL") << "\"\n";
    }
}
ostream &EqExtEOS::PrintComponents(ostream &out)
{
    if( exteos )
        exteos->PrintAll(out);
    return out;
}

int EqExtEOS::ConvertParams(Convert &convert)
{
    if( ExtEOS::ConvertParams(convert) )
        return 1;
    return exteos ? exteos->ConvertUnits(convert) : 1;
}


//
// EOS functions
//
double *EqExtEOS::z_f(double V, double e)
{
    if( exteos == NULL )
        return NULL;
    double *zref = exteos->z_f(V,e);
    if( zref == NULL )
        return NULL;
    int i;
    for( i=0; i<n; ++i )
        ztmp[i] = zref[i];
    return Equilibrate(V,e,ztmp) ? NULL : ztmp;
}

double EqExtEOS::P(double V, double e)
{
    return exteos ? exteos->P(V, e, z_f(V,e)) : NaN;
}

double EqExtEOS::T(double V, double e)
{
    return exteos ? exteos->T(V, e, z_f(V,e)) : NaN;
}

double EqExtEOS::S(double V, double e)
{
    return exteos ? exteos->S(V, e, z_f(V,e)) : NaN;
}

double EqExtEOS::c2(double V, double e)
{
    return exteos ? exteos->c2(V, e, z_f(V,e)) : NaN;
}

double EqExtEOS::Gamma(double V, double e)
{
    return exteos ? exteos->Gamma(V, e, z_f(V,e)) : NaN;
}

double EqExtEOS::CV(double V, double e)
{
    return exteos ? exteos->CV(V, e, z_f(V,e)) : NaN;
}

double EqExtEOS::CP(double V, double e)
{
    return exteos ? exteos->CP(V, e, z_f(V,e)) : NaN;
}

double EqExtEOS::KT(double V, double e)
{
    return exteos ? exteos->KT(V, e, z_f(V,e)) : NaN;
}

double EqExtEOS::beta(double V, double e)
{
    return exteos ? exteos->beta(V, e, z_f(V,e)) : NaN;
}

double EqExtEOS::FD(double V, double e)
{
    return exteos ? exteos->FD(V, e, z_f(V,e)) : NaN;
}

int EqExtEOS::NotInDomain(double V, double e)
{
    return exteos ? exteos->NotInDomain(V, e, z_f(V,e)) : 1;
}
//
// Extended functionality
// 
double EqExtEOS::P(double V, double e, const double *z)
{
    return exteos ? exteos->P(V, e, z) : NaN;
}

double EqExtEOS::T(double V, double e, const double *z)
{
    return exteos ? exteos->T(V, e, z) : NaN;
}

double EqExtEOS::S(double V, double e, const double *z)
{
    return exteos ? exteos->S(V, e, z) : NaN;
}

double EqExtEOS::c2(double V, double e, const double *z)
{
    return exteos ? exteos->c2(V, e, z) : NaN;
}

// FD = -0.5*(1/(rho*c)^2)*(V*d/dV)(rho*c)^2
double EqExtEOS::FD(double V, double e, const double *z)
{
    return exteos ? exteos->FD(V, e, z) : NaN;
}

double EqExtEOS::Gamma(double V, double e, const double *z)
{
    return exteos ? exteos->Gamma(V, e, z) : NaN;
}

double EqExtEOS::CV(double V, double e, const double *z)
{
    return exteos ? exteos->CV(V, e, z) : NaN;
}

double EqExtEOS::CP(double V, double e, const double *z)
{
    return exteos ? exteos->CP(V, e, z) : NaN;
}

double EqExtEOS::KT(double V, double e, const double *z)
{
    return exteos ? exteos->KT(V, e, z) : NaN;
}

double EqExtEOS::beta(double V, double e, const double *z)
{
    return exteos ? exteos->beta(V, e, z) : NaN;
}

int EqExtEOS::NotInDomain(double V, double e, const double *z)
{
    return exteos ? exteos->NotInDomain(V, e, z) : 1;
}

int EqExtEOS::Rate(double V, double e, const double *z, double *zdot)
{
    return -1;  // error
}

int EqExtEOS::TimeStep(double V, double e, const double *z, double &dt)
{
    dt = 0;
    return 2;
}

int EqExtEOS::Integrate(double V, double e, double *z, double dt)
{
    return exteos ? exteos->Equilibrate(V, e, z) : 1;
}

int EqExtEOS::Equilibrate(double V, double e, double *z)
{
    return exteos ? exteos->Equilibrate(V, e, z) : 1;
}

int EqExtEOS::var(const char *name, double V, double e, const double *z,
                      double &value)
{
    return exteos ? exteos->var(name,V,e,z,value) : 1;
}
