#include <LocalMath.h>

#include "EOS.h"
#include "Detonation_gen.h"

Detonation_gen::Detonation_gen(EOS &e, double pvac)
    : Detonation(e,pvac), ODE(1,128)
{
    //rel_tol      = 1.e-6;
    //ODE::epsilon = 1.e-6;
    rel_tol      = e.OneDFunc_rel_tol;
    ODE::epsilon = e.ODE_epsilon;
}

Detonation_gen::~Detonation_gen()
{
    // Null
}

int Detonation_gen::F(double *y_prime, const double *y, double V)
{
    double e = y[0];
    double P = eos->P(V,e);
    double Gamma = eos->Gamma(V,e);
    double c2 = eos->c2(V,e);
    double dV = V0-V;
    y_prime[0] = -0.5*(P+P0 +(c2/V-Gamma*P)*dV/V)/(1.-0.5*Gamma*dV/V);
    return isnan(y_prime[0]);
}

class Detonation_gen_CJ : public ODEfunc
{
public:
    Detonation_gen *det;
    Detonation_gen_CJ(Detonation_gen *Det) : det(Det) {}
    double f(double V, const double *y, const double *yp);
};
double Detonation_gen_CJ::f(double V, const double *y, const double *yp)
{
    double e  = y[0];
    double P  =  det->Eos().P(V,e);
    double c2 =  det->Eos().c2(V,e);
    return V*V*(P-det->P0) - c2*(det->V0-V);
}
int Detonation_gen::InitCJ()
{
    Pw = eos->P(V0,e0);
    if( isnan(Pw) || Pw <= P0 )
        return 1;
    e[0] = e0;
    double dV = -0.01*V0;
    int status;
    if( (status=ODE::Initialize(V0,e,dV)) )
    {
        eos->ErrorHandler()->Log("Detonation_gen::InitCJ", __FILE__, __LINE__,
                eos, "Initialize failed with status %s\n", ErrorStatus(status) );
        return 1;
    }
    Detonation_gen_CJ CJ(this);
    double val=0.0;
    if( (status=Integrate(CJ, val, Vcj, e)) )
    {
        eos->ErrorHandler()->Log("Detonation_gen::InitCJ", __FILE__, __LINE__,
                eos, "CJ failed with status %s\n", ErrorStatus(status) );
        return 1;
    }
    ecj = e[0];
    Pcj = eos->P(Vcj,ecj);
    Dcj = V0*sqrt((Pcj-P0)/(V0-Vcj));
    ucj = (1.-Vcj/V0)*Dcj;
    return 0;
}


class Detonation_gen_P : public ODEfunc
{
public:
    Detonation_gen *det;
    Detonation_gen_P(Detonation_gen *Det) : det(Det) {}
    double f(double V, const double *y, const double *yp);
};
double Detonation_gen_P::f(double V, const double *y, const double *yp)
{
    double e  = y[0];
    return -det->Eos().P(V,e);
}
int Detonation_gen::P(double p,  int dir, WaveState &wave)
{
    if( isnan(P0) )
        return 1;
    if( abs(Pcj - p) < rel_tol*Pcj  )
        return CJwave( dir, wave );
    //if( p < Pcj )
    if( p <= Pw )
        return 1;  
    Detonation_gen_P det(this);
    double val=-p;
    int status = Integrate(det, val, wave.V, e);
    if( status )
    {
        eos->ErrorHandler()->Log("Detonation_gen::P", __FILE__, __LINE__,
                eos, "det failed with status %s\n", ErrorStatus(status) );
        return 1;
    }
// set shock state  
    wave.e  = e[0];
    wave.P  = eos->P(wave.V,wave.e);
    double D = dir * V0 * sqrt( (wave.P-P0)/(V0-wave.V) );
    wave.u  = u0 + (1.-wave.V/V0)*D;
    wave.us = u0 + D;         
    return 0;
}

class Detonation_gen_u2 : public ODEfunc
{
public:
    Detonation_gen *det;
    Detonation_gen_u2(Detonation_gen *Det) : det(Det) {}
    double f(double V, const double *y, const double *yp);
};
double Detonation_gen_u2::f(double V, const double *y, const double *yp)
{
    double e  = y[0];
    double P = det->Eos().P(V,e);
    return -(P-det->P0)*(det->V0-V);
}
int Detonation_gen::u(double u1,  int dir, WaveState &wave)
{
    if( isnan(P0) )
        return 1;
    wave.u  = u1;
    u1 = dir*(u1-u0);
    if( abs(ucj - u1) < rel_tol*ucj  )
        return CJwave( dir, wave );
    //if( u1 < ucj )
    if( u1 <= 0.0 )
        return 1;
    Detonation_gen_u2 det(this);
    double val=-u1*u1;
    int status = Integrate(det, val, wave.V, e);
    if( status )
    {
        eos->ErrorHandler()->Log("Detonation_gen::u", __FILE__, __LINE__,
                eos, "det failed with status %s\n", ErrorStatus(status) );
        return 1;
    }
// set shock state  
    wave.e  = e[0];
    wave.P  = eos->P(wave.V,wave.e);
    wave.us = u0 + dir * V0/(V0-wave.V) * u1;         
    return 0;
}

class Detonation_gen_us : public ODEfunc
{
public:
    Detonation_gen *det;
    Detonation_gen_us(Detonation_gen *Det) : det(Det) {}
    double f(double V, const double *y, const double *yp);
};
double Detonation_gen_us::f(double V, const double *y, const double *yp)
{
    if( det->Vcj < V )
    { // weak branch
      return -sqr(det->Dcj/det->V0) * det->Vcj/V;
    }
    double e  = y[0];
    double P = det->Eos().P(V,e);
    return -(P-det->P0)/(det->V0-V);
}
int Detonation_gen::u_s(double us, int dir, WaveState &wave)
{
    if( isnan(P0) )
        return 1;
    wave.us = us;  
    us = dir*(us-u0);
    if( abs(Dcj - us) < rel_tol*Dcj  )
        return CJwave( dir, wave );
    if( us < Dcj )
        return 1;
    Detonation_gen_us det(this);
    double val=-sqr(us/V0);
    int status = Integrate(det, val, wave.V, e);
    if( status )
    {
        eos->ErrorHandler()->Log("Detonation_gen::us", __FILE__, __LINE__,
                eos, "det failed with status %s\n", ErrorStatus(status) );
        return 1;
    }
// set shock state  
    wave.e  = e[0];
    wave.P  = eos->P(wave.V,wave.e);
    wave.u  = u0 + dir*(1. - wave.V/V0)*us;
    return 0;
}

class Detonation_gen_weak_us : public ODEfunc
{
public:
    Detonation_gen *det;
    Detonation_gen_weak_us(Detonation_gen *Det) : det(Det) {}
    double f(double V, const double *y, const double *yp);
};
double Detonation_gen_weak_us::f(double V, const double *y, const double *yp)
{
    double ans;
    if( V < det->Vcj )
    { // strong branch
      ans = sqr(det->Dcj/det->V0) * V/det->Vcj;
    }
    else if( det->V0 <= V )
       ans = 1.e16*sqr(det->Dcj/det->V0); 
    else {
        double e  = y[0];
        double P = det->Eos().P(V,e);
        ans = (P-det->P0)/(det->V0-V);
    }
    return ans;
}
int Detonation_gen::w_u_s(double us, int dir, WaveState &wave)
{
    if( isnan(P0) )
        return 1;
    wave.us = us;  
    us = dir*(us-u0);
    if( abs(Dcj - us) < rel_tol*Dcj  )
        return CJwave( dir, wave );
    if( us < Dcj )
        return 1;
    Detonation_gen_weak_us det(this);
    double val=sqr(us/V0);
    int status = Integrate(det, val, wave.V, e);
    if( status )
    {
        eos->ErrorHandler()->Log("Detonation_gen::us", __FILE__, __LINE__,
                eos, "det failed with status %s\n", ErrorStatus(status) );
        return 1;
    }
// set shock state  
    wave.e  = e[0];
    wave.P  = eos->P(wave.V,wave.e);
    wave.u  = u0 + dir*(1. - wave.V/V0)*us;
    return 0;
}


int Detonation_gen::V(double v1,  int dir, WaveState &wave)
{
    if( isnan(P0) )
        return 1;
    if( abs(Vcj - v1) < rel_tol*Vcj )
        return CJwave( dir, wave );
    //if( Vcj < v1 )
    if( V0 <= v1 )
        return 1;
    wave.V = v1;
    int status = Integrate(wave.V, e);
    if( status )
    {
        eos->ErrorHandler()->Log("Detonation_gen::V", __FILE__, __LINE__,
                eos, "det failed with status %s\n", ErrorStatus(status) );
        return 1;
    }
    wave.e  = e[0];
    wave.P  = eos->P(wave.V,wave.e);
    double dV = V0-wave.V;
    double D = dir * V0 * sqrt( (wave.P-P0)/dV );
    wave.us = u0 + D;   
    wave.u  = u0 + (dV/V0)*D;
    return 0;
}
