#include "ExtEOS_VT.h"

ExtEOS_VT::ExtEOS_VT(const char *eos_type) : EOS_VT(eos_type), n(0), ztmp(NULL)
{
    // Null
}

ExtEOS_VT::ExtEOS_VT(const ExtEOS_VT &e_eos) : EOS_VT(e_eos)
{
    n = e_eos.n;
    ztmp = (n>0) ? new double[n] : NULL;
    int i;
    for( i=0; i<n; i++ )
        ztmp[i] = e_eos.ztmp[i];        
}

ExtEOS_VT::~ExtEOS_VT()
{
    delete ztmp;
}

ExtEOS_VT *ExtEOS_VT::Duplicate(const double *)
{
    return NULL;
}

IDOF *ExtEOS_VT::z_ref()
{
    return NULL;
}
//
// EOS_VT functions
//
double *ExtEOS_VT::z_f(double V, double T)
{
    IDOF *zref = z_ref();
    if( zref == NULL )
        return NULL;

    return &(*zref);
}

double ExtEOS_VT::P(double V, double T)
{
    return P(V, T, z_f(V,T));
}

double ExtEOS_VT::e(double V, double T)
{
    return e(V, T, z_f(V,T));
}

double ExtEOS_VT::S(double V, double T)
{
    return S(V, T, z_f(V,T));
}

double ExtEOS_VT::P_V(double V, double T)
{
    return P_V(V, T, z_f(V,T));
}

double ExtEOS_VT::P_T(double V, double T)
{
    return P_T(V, T, z_f(V,T));
}

double ExtEOS_VT::CV(double V, double T)
{
    return CV(V, T, z_f(V,T));
}

double ExtEOS_VT::F(double V, double T)
{
    return F(V,T, z_f(V,T));
}

int ExtEOS_VT::NotInDomain(double V, double T)
{
    return NotInDomain(V, T, z_f(V,T));
}

double ExtEOS_VT::CP(double V, double T)
{
    return CP(V, T, z_f(V,T));
}

double ExtEOS_VT::KS(double V, double T)
{
    return KS(V, T, z_f(V,T));
}

double ExtEOS_VT::KT(double V, double T)
{
    return KT(V, T, z_f(V,T));
}

double ExtEOS_VT::Gamma(double V, double T)
{
    return Gamma(V, T, z_f(V,T));
}
double ExtEOS_VT::beta(double V, double T)
{
    return beta(V, T, z_f(V,T));
}

double ExtEOS_VT::c2(double V, double T)
{
    return c2(V, T, z_f(V,T));
}
//
// Extended functionality
// 
double ExtEOS_VT::e(double V, double T, const double *z)
{
    return NaN;
}

double ExtEOS_VT::S(double V, double T, const double *z)
{
    return NaN;
}

double ExtEOS_VT::P_V(double V, double T, const double *z)
{
    return NaN;
}

double ExtEOS_VT::P_T(double V, double T, const double *z)
{
    return NaN;
}


double ExtEOS_VT::CV(double V, double T, const double *z)
{
    return NaN;
}

double ExtEOS_VT::F(double V, double T, const double *z)
{
    return e(V,T, z) - T*S(V,T, z);
}

int ExtEOS_VT::NotInDomain(double V, double T, const double *z)
{
    if( V <= 0 || T < 0 )
        return 1;
    double csq = cT2(V,T, z);
    return (isnan(csq) || csq <= 0.0) ? 1 : 0;
}

double ExtEOS_VT::CP(double V, double T, const double *z)
{
    return CV(V,T, z) + V*T/KT(V,T, z)*sqr(P_T(V,T, z));
}

double ExtEOS_VT::KS(double V, double T, const double *z)
{
    return -V*P_V(V,T, z) + T*V/CV(V,T, z)*sqr(P_T(V,T, z));
}

double ExtEOS_VT::KT(double V, double T, const double *z)
{
    return -V*P_V(V,T, z);
}

double ExtEOS_VT::Gamma(double V, double T, const double *z)
{
    return V*P_T(V,T, z)/CV(V,T, z);
}

double ExtEOS_VT::beta(double V, double T, const double *z)
{
    return P_T(V,T, z)/KT(V,T, z);
}

double ExtEOS_VT::c2(double V, double T, const double *z)
{
    return V*KS(V,T, z);
}

double ExtEOS_VT::cT2(double V, double T, const double *z)
{
    return V*KT(V,T, z);
}


double ExtEOS_VT::cT2(double V, double T)
{
    return V*KT(V,T);
}



ExtEOS_VT *FetchExtEOS_VT(const char *type, const char *name, DataBase &db)
{
    EOS_VT *eos = static_cast<EOS_VT*>(db.FetchObj("EOS_VT",type,name));
    if( eos == NULL )
        return NULL;
    ExtEOS_VT *ext = dynamic_cast<ExtEOS_VT*>(eos);
  /***
    if( ext == NULL )
    {
        ext = new Hydro_VT(*eos);
        deleteEOS_VT(eos);
    }
  **/
    return ext;
}

ExtEOS_VT *FetchNewExtEOS_VT(const char *type, const char *name, DataBase &db)
{
    EOS_VT *eos = static_cast<EOS_VT*>(db.FetchNewObj("EOS_VT",type,name));
    if( eos == NULL )
        return NULL;
    ExtEOS_VT *ext = dynamic_cast<ExtEOS_VT*>(eos);
  /***
    if( ext == NULL )
    {
        ext = new Hydro_VT(*eos);
        deleteEOS_VT(eos);
    }
  **/
    return ext;
}

ExtEOS_VT *FetchExtEOS_VT(const char *type_name, DataBase &db)
{
    EOS_VT *eos = static_cast<EOS_VT*>(db.FetchObj("EOS_VT",type_name));
    if( eos == NULL )
        return NULL;
    ExtEOS_VT *ext = dynamic_cast<ExtEOS_VT*>(eos);
  /***
    if( ext == NULL )
    {
        ext = new Hydro_VT(*eos);
        deleteEOS_VT(eos);
    }
  **/
    return ext;
}

ExtEOS_VT *FetchNewExtEOS_VT(const char *type_name, DataBase &db)
{
    EOS_VT *eos = static_cast<EOS_VT*>(db.FetchNewObj("EOS_VT",type_name));
    if( eos == NULL )
        return NULL;
    ExtEOS_VT *ext = dynamic_cast<ExtEOS_VT*>(eos);
  /***
    if( ext == NULL )
    {
        ext = new Hydro_VT(*eos);
        deleteEOS_VT(eos);
    }
  **/
    return ext;
}
