#include <LocalMath.h>

#include "EOS.h"
#include "Detonation_gen.h"



class CJstate
{
public:
    WaveState CJ;       // CJ state

    double V0;          // V0 of reactants
    double e0;          // e0 of reactants
    double P0;          // P0 of reactants

    // constant volume state
    double Pcv;         // eos.P(V0,e0) > P0
    double Kcv;         // eos.K(V0,e0)

    EOS &eos;           // products HE

    double tol_iter;    // relative tolerance for iteration
    int    max_iter;    // maximum number of iterations

    CJstate(EOS &prod, double V0, double e0, double P0);
    ~CJstate();

    // sonic point on Hugoniot
    int sonic();
    // point on Hugoniot for given V
    int HugV( double  V, double &e, double &P, double &u );

    int status;
    const char *ErrorStatus();
};

const char *CJstate::ErrorStatus()
{
	const char *string;
	
	switch(status)
	{
	case 0:
		string = "OK (no error)";
		break;
	case 1:
		string = "Pcv failed";
		break;
	case 2:
		string = "Pcv <= P0";
		break;
	case 3:
		string = "Kcv failed";
		break;
    case 4:
		string = "sonic, bounds failed";
		break;
    case 5:
		string = "sonic, failed to meet tolerance";
		break;
    case 6:
		string = "HugV, failed to meet tolerance";
		break;

	default:
		string = "unknown error status";
	}
	return string;
}

CJstate::CJstate(EOS &prod, double V_0, double e_0, double P_0)
                : eos(prod), tol_iter(1.e-14), max_iter(100)
{
    V0 = V_0;
    e0 = e_0;
    P0 = P_0;
    Pcv = eos.P(V0,e0);
    if( std::isnan(Pcv) )
    {
        status = 1;
        return;
    }
    if( Pcv <= P0 )
    {
        status = 2;
        return;
    }
    Kcv = eos.KS(V0,e0); // isentropic bulk modulus
    if( std::isnan(Kcv) )
    {
        status = 3;
        return;
    }
    status = 0;
}

CJstate::~CJstate()
{
}

int CJstate::sonic()
{
    double V,e,P,u;
    //
    // initial bounds (V2 < V1)
    double V1 = V0;
    double V2;
    int count = max_iter;
    while( count-- )
    {
       V2 = 0.9*V1;
       if( HugV(V2, e, P, u) ) return status;
       double m_2 = (P-P0)/(V0-V2);
       double c2  = eos.c2(V2,e);
       double sonic = V2*V2*m_2 - c2;
       if( sonic >= 0.0 ) { V1 = V2; }
       else { break; }
    }
    if( count <= 0 )
    {
       status = 4;
       return 1;
    }
    //
    // bisection
    while( count-- )
    {
       V = 0.5*(V1+V2);
       (void) HugV(V, e, P, u);
       double m_2 = (P-P0)/(V0-V);
       double c2  = eos.c2(V,e);
       double sonic = V*V*m_2 - c2;
       if( std::abs(sonic) < tol_iter*c2 ) break;
       if( sonic < 0. ) V2 = V;
       else V1 = V;
    }
    CJ.V = V;
    CJ.e = e;
    CJ.u = u;
    CJ.P = P;
    CJ.us = V0*sqrt( (P-P0)/(V0-V) );
    status = (count>=0) ? 0 : 5;
    return status;
}
int CJstate::HugV(double V, double &e, double &P, double &u)
{
    e = e0 + (V0-V)*(P0+Kcv*(1.-V/V0)); // initial guess for Newton iteration
    int count = max_iter;
    while( count-- )
    {
       P = eos.P(V,e);
       double h = e-e0 - 0.5*(P+P0)*(V0-V);
       if( std::abs(h) < tol_iter*(e-e0) ) break;
       double Gamma =  eos.Gamma(V,e);
       double de    = h/(1.-0.5*Gamma*(V0-V)/V);
       e -= de;
    }
    u = sqrt( (P-P0)*(V0-V) );
    status = (count>=0) ? 0 : 6;
    return status;
}
//
//
//
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
    return std::isnan(y_prime[0]);
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
    CJstate CJ(*eos,V0,e0,P0);
    if( CJ.status || CJ.sonic() )
    {
        eos->ErrorHandler()->Log("Detonation_gen::InitCJ", __FILE__, __LINE__,
                eos, "CJstate failed with status %s\n", CJ.ErrorStatus() );
        return 1;
    }
    Vcj = CJ.CJ.V;
    ecj = CJ.CJ.e;
    ucj = CJ.CJ.u;
    Pcj = CJ.CJ.P;
    Dcj = CJ.CJ.us;
    //
    // ToDo: solve jump conditions instead of using ODE solver
    // initialize ODE solver
    //
    e[0] = ecj;
    double dV = -0.01*Vcj;
    int status;
    if( (status=ODE::Initialize(Vcj,e,dV)) )
    {
        eos->ErrorHandler()->Log("Detonation_gen::InitCJ", __FILE__, __LINE__,
                eos, "Initialize failed with status %s\n", ErrorStatus(status) );
        return 1;
    }

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
    if( std::isnan(P0) )
        return 1;
    if( std::abs(Pcj - p) < rel_tol*Pcj  )
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
    if( std::isnan(P0) )
        return 1;
    wave.u  = u1;
    u1 = dir*(u1-u0);
    if( std::abs(ucj - u1) < rel_tol*ucj  )
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
    if( std::isnan(P0) )
        return 1;
    wave.us = us;  
    us = dir*(us-u0);
    if( std::abs(Dcj - us) < rel_tol*Dcj  )
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
    if( std::isnan(P0) )
        return 1;
    wave.us = us;  
    us = dir*(us-u0);
    if( std::abs(Dcj - us) < rel_tol*Dcj  )
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
    if( std::isnan(P0) )
        return 1;
    if( std::abs(Vcj - v1) < rel_tol*Vcj )
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
