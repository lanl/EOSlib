#include "ExtEOS.h"
#include <cstring>
//
// EOS functions
//
int ExtEOS::set_z(const char *var, double value, double *z)
{
    if( z==NULL || z_ref==NULL ) return -1;
    int i = z_ref->var_index(var);
    if(i<0) return 1;
    z[i] = value;
    return 0;
}
int ExtEOS::get_z(double *z, const char *var, double &value)
{
    if( z==NULL || z_ref==NULL ) return -1;
    int i = z_ref->var_index(var);
    if(i<0) return 1;
    value = z[i];
    return 0;
}

double *ExtEOS::z_f(double V, double e)
{
    int i;
    for( i=0; i<n; i++ )
        ztmp[i] = (*z_ref)(i);
    if( !frozen )
    {// equilibrium
        if( Equilibrate(V,e,ztmp) )
        {
            EOSerror->Log("ExtEOS::z_f",__FILE__,__LINE__,this,
                          "Equilibrate failed\n" );
            return NULL;     
        }
    }
    return ztmp;
}

double ExtEOS::P(double V, double e)
{
    if( n>0 && z_f(V,e)==NULL )
        return NaN;
    return P(V, e, ztmp);
}

double ExtEOS::T(double V, double e)
{
    if( n>0 && z_f(V,e)==NULL )
        return NaN;
    return T(V, e, ztmp);
}

double ExtEOS::S(double V, double e)
{
    if( n>0 && z_f(V,e)==NULL )
        return NaN;
    return S(V, e, ztmp);
}

double ExtEOS::c2(double V, double e)
{
    if( frozen )
    {
        if( n>0 && z_f(V,e)==NULL )
            return NaN;
        return c2(V, e, ztmp);
    }
    else
        return EOS::c2(V,e);
    // equilibrium need not be adiabatic
    // affects sound speed needed for Hugoniot
    // de = -P dV + T dS + e_z dz
}

double ExtEOS::Gamma(double V, double e)
{
    if( n>0 && z_f(V,e)==NULL )
        return NaN;
    return Gamma(V, e, ztmp);
}

double ExtEOS::CV(double V, double e)
{
    if( n>0 && z_f(V,e)==NULL )
        return NaN;
    return CV(V, e, ztmp);
}

double ExtEOS::CP(double V, double e)
{
    if( n>0 && z_f(V,e)==NULL )
        return NaN;
    return CP(V, e, ztmp);
}

double ExtEOS::KT(double V, double e)
{
    if( n>0 && z_f(V,e)==NULL )
        return NaN;
    return KT(V, e, ztmp);
}

double ExtEOS::beta(double V, double e)
{
    if( n>0 && z_f(V,e)==NULL )
        return NaN;
    return beta(V, e, ztmp);
}

double ExtEOS::FD(double V, double e)
{
    if( n>0 && z_f(V,e)==NULL )
        return NaN;
    return FD(V, e, ztmp);
}

double ExtEOS::e_PV(double p, double V)
{
    if( ! frozen )
        return NaN;
    if( n>0  )
    {
        int i;
        for( i=0; i<n; i++ )
            ztmp[i] = (*z_ref)(i);
    }
    return e_PV(p, V, ztmp);
}

int ExtEOS::NotInDomain(double V, double e)
{
    if( n>0 && z_f(V,e)==NULL )
        return 1;
    return NotInDomain(V, e, ztmp);
}
//
// Extended functionality
// 
double  ExtEOS::e_PV(double p, double V, const double *z)
{
// minimal error checking for efficiency
// fails when p < P_c(V,z), where P_c is cold curve
    double e = e_ref;
    if( V < V_ref )
        e += p*(V_ref-V);
    int count = Iter_max;
    double tol =  max( OneDFunc_abs_tol, OneDFunc_rel_tol*p );
    while( --count )
    {
        double p1 = P(V,e,z);
        if( std::isnan(p1) )
            break;
        if( abs(p-p1) < tol )
            return e;
        e += V * (p - p1) / Gamma(V,e,z);
    }
    return NaN;
}

double ExtEOS::T(double V, double e, const double *z)
{
    return NaN;
}

double ExtEOS::S(double V, double e, const double *z)
{
    return NaN;
}

double ExtEOS::c2(double V, double e, const double *z)
{
    double p0 = P(V,e, z);
    double dV = dV_factor*V;
    
    double p1,p2;

    int count;
    for(count=10; count-->0; dV *= 0.25 )
    { 
        double V1 = V - dV;
        double e1 = e + p0*dV;
        double p1 = P(V1,e1, z);
        if( p1 <= p0 )
            continue;

        double V2 = V1 - dV;
        double e2 = e1 + p1*dV;
        double p2 = P(V2,e2, z);    
        if( p2 > p1 )
        {
            double dP = 0.5*(p2-p0);
            double d2P = (p2-2*p1+p0);
            if( abs(d2P) < c2_tol*dP )
                return V*V*(dP + d2P)/dV;
        }
    }
    return EOSerror->Log("ExtEOS::c2", __FILE__, __LINE__, this, "failed");
}

// FD = -0.5*(1/(rho*c)^2)*(V*d/dV)(rho*c)^2
double ExtEOS::FD(double V, double e, const double *z)
{
    double p0 = P(V,e, z);
    double rc2_0 = c2(V,e, z)/sqr(V);
    double dV = dV_factor*V;
    
    double p1,p2;
    int count;
    for(count=10; count-->0; dV *= 0.25 )
    { 
        double V1 = V - dV;
        double e1 = e + p0*dV;
        double p1 = P(V1,e1, z);
        if( p1 <= p0 )
            continue;
        double rc2_1 = c2(V1,e1, z)/sqr(V1);

        double V2 = V1 - dV;
        double e2 = e1 + p1*dV;
        double p2 = P(V2,e2, z);    
        if( p2 > p1 )
        {
            double rc2_2 = c2(V2,e2, z)/sqr(V2);
            double drc2 = 0.5*(rc2_2-rc2_0);
            double d2rc2 = (rc2_2-2*rc2_1+rc2_0);
            if( abs(d2rc2) < FD_tol*abs(drc2) )
                return 0.5*(drc2 + d2rc2)/dV *(V/rc2_0);
        }
    }
    return EOSerror->Log("ExtEOS::FD", __FILE__, __LINE__, this,
                  "failed V=%F, e=%F", V, e);
}

double ExtEOS::Gamma(double V, double e, const double *z)
{
    double p0=P(V,e, z);
    double de = de_factor*c2(V,e, z);
    
    int count;
    for(count=20; count-->0; de *= 0.25 )
    { 
        double e1 = e + de;
        double p1 = P(V,e1, z);

        double e2 = e1 + de;
        double p2 = P(V,e2, z);    

        double dP = 0.5*(p2-p0);
        double d2P = (p2-2*p1+p0);
        if( abs(d2P) < Gamma_tol*dP )
            return V*(dP - d2P)/de;
    }
    return EOSerror->Log("ExtEOS::Gamma", __FILE__, __LINE__, this, "failed");
}

double ExtEOS::CV(double V, double e, const double *z)
{
    double T0 = T(V,e, z);
    if( std::isnan(T0) )
        return NaN;
    double de = de_factor*c2(V,e, z);
    
    int count;
    for(count=20; count-->0; de *= 0.25 )
    { 
        double e1 = e + de;
        double T1 = T(V,e1, z);

        double e2 = e1 + de;
        double T2 = T(V,e2, z);    

        double dT = 0.5*(T2-T0);
        double d2T = (T2-2*T1+T0);
        if( abs(d2T) < CV_tol*dT )
            return de/(dT - d2T);
    }
    return EOSerror->Log("ExtEOS::CV", __FILE__, __LINE__, this, "failed");
}


double ExtEOS::CP(double V, double e, const double *z)
{
    double csq = c2(V,e,z);
    double G   = Gamma(V,e,z);
    double Cv  = CV(V,e,z);
    return csq*Cv/(csq - G*G*Cv*T(V,e,z));
}

double ExtEOS::KT(double V, double e, const double *z)
{
    double G   = Gamma(V,e, z);
    return (c2(V,e, z) -  G*G*CV(V,e, z)*T(V,e, z))/V;
}

double ExtEOS::beta(double V, double e, const double *z)
{
    double G   = Gamma(V,e, z);
    double Cv  = CV(V,e, z);
    return G*Cv/(c2(V,e, z)-G*G*Cv*T(V,e, z));
}

int ExtEOS::NotInDomain(double V, double e, const double *z)
{
    if( V <= 0 )
        return 1;
    double csq = c2(V,e, z);
    return (std::isnan(csq) || csq <= 0.0) ? 1 : 0;
}
//
// dummy functions
//
int ExtEOS::Rate(double V, double e, const double *z, double *zdot)
{
    return -1;  // error
}
int ExtEOS::TimeStep(double V, double e, const double *z, double &dt)
{
    return -1;  // error
}
int ExtEOS::Integrate(double V, double e, double *z, double dt)
{
    return 1;   // error
}
int ExtEOS::Equilibrate(double V, double e, double *z)
{
    return 1;   // error
}
//
int ExtEOS::var(const char *name, double V, double e, const double *z,
                    double &value)
{
    if( z_ref==NULL || z==NULL )
        return -1;
    if( !strcmp(name,"timestep") )
        return TimeStep(V,e,z,value);
    int ddt = 0;
    if( !strncmp(name,"ddt_",4) )
    {
        ddt = 1;
        name += 4;
    }
    int j = z_ref->var_index(name);
    if( j == -1 )
        return 1;
    if( ddt == 0 )
        value = z[j];
    else
    {
        if( Rate(V,e,z,ztmp) != n )
            return -1;
        value = ztmp[j];
    }
    return 0;
}
//
int ExtEOS::ddz(int i, double V, double e, const double *z,
                       double &dP, double &dT)
{
    return 1;
}
