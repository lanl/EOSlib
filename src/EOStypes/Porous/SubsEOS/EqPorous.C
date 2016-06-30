#include "Porous.h"
#include <Isentrope_ODE.h>

// DEBUG
//#include <iostream>
//using namespace std;

int EqPorous::InitPhi()
{
    phi_max = phi_eq->phi_1();
    double e[3];
    if( phi_eq->energy(phi_max,e) )
        return 1;
        
    B_max      = e[0];
    dBdphi_max = e[1];
    
    V_f = e_f = NaN;
    return 0;
}

EqPorous::~EqPorous()
{
    deletePhiEq(phi_eq);
    deleteEOS(solid);
}

double EqPorous::f(double phi)
{
    double e[3];
    if( phi_eq->energy(phi,e) )
        return EOSerror->Log("EqPorous::f", __FILE__, __LINE__, this,
                   "phi_eq->energy failed");
    
    phi_f = phi;
      B_f = e[0];
      P_f = phi_f*solid->P(phi_f*V_f, e_f-B_f);
    // debug
    //double fphi = V_f*P_f - phi*e[1];
    //if( std::isnan(fphi) )
    //    return EOSerror->Log("EqPorous::f", __FILE__, __LINE__, this,
    //           "debug, V,e,B, phi = %e,%e,%e,%e\n",V_f,e_f,B_f,phi_f);
    //return fphi;
    return V_f*P_f - phi*e[1];    
}

double EqPorous::P(double V, double e)
{
    if( V == V_f && e == e_f )
        return P_f;
    V_f = V;
    e_f = e;

    // ToDo: assumes f(phi) is monotonic
    //       not true for some cases with UsUp solid EOS
    //       Take solution closest to phi_1
    
    double phi_1 = phi_eq->phi_1();
    double f_hi = f(phi_1);
    if( f_hi >= 0.0 )
        return P_f;
    double phi_hi = phi_1;
    
    double phi_0 = phi_eq->phi_0();
    double f_lo;
    double phi_lo;
    int i;
    for( i=1; i<=10; ++i)
    {
        phi_lo = (i*phi_0 +(10-i)*phi_1)/10;
        f_lo = f(phi_lo);
        if( f_lo == 0.0 )
            return P_f;
        if( f_lo > 0.0 ) break;
        phi_hi = phi_lo;
        f_hi   = f_lo;
    }
    if( f_lo <= 0.0)
        return  phi_eq->P_min();        // minimum pressure
    
    double phi = zero(phi_lo, f_lo, phi_hi, f_hi);
    if( OneDFunction::Status() )
        return NaN;
    //    return EOSerror->Log("EqPorous::P", __FILE__, __LINE__, this,
    //               "OneDFunction status: %s\n", ErrorStatus());
    
    (void) f(phi);
    return P_f;
}


double EqPorous::T(double V, double e)
{
    (void) P(V,e);
    return solid->T(phi_f*V, e-B_f);
}

double EqPorous::S(double V, double e)
{
    (void) P(V,e);
    return solid->S(phi_f*V, e-B_f);
}

double EqPorous::c2(double V, double e)
{
    (void) P(V,e);
        
    double c2_s = solid->c2(phi_f*V, e-B_f);
    
    double B[3];
    (void) phi_eq->energy(phi_f,B);
    
    double g = c2_s- P_f*V;
    return c2_s - g*g/(c2_s + phi_f*phi_f*B[2]);
        
    // note: for c2 -> c2_s as phi -> 1
    //       need d(B)/d(phi) and d^2(B)/d(phi)^2 -> infty.
}

double EqPorous::Gamma(double V, double e)
{
    (void) P(V,e);
    
    double e_s = e-B_f;
    double V_s = phi_f*V;
    double Gamma_s = solid->Gamma(V_s,e_s);

    /***** continuity requires d^2(B)/d(phi)^2 -> infty as phi -> 1
    if( phi_f == 1 )
        return Gamma_s;
    *****/

    double c2_s = solid->c2(V_s,e_s);
    
    double B[3];
    (void) phi_eq->energy(phi_f,B);
    
    double phi2B2 = phi_f*phi_f*B[2];
    
    return (P_f*V + phi2B2)*Gamma_s/(c2_s + phi2B2);
}


double EqPorous::CV(double V, double e)
{
    (void) P(V,e);
    
    double e_s = e-B_f;
    double V_s = phi_f*V;
    double Cv_s = solid->CV(V_s,e_s);
    
    /***** continuity requires d^2(B)/d(phi)^2 -> infty as phi -> 1
    if( phi_f == 1 )
        return Cv_s;
    *****/
    
    double c2_s    = solid->c2(V_s,e_s);
    double Gamma_s = solid->Gamma(V_s,e_s);
    double Ts      = solid->T(V_s,e_s);
    
    double B[3];
    (void) phi_eq->energy(phi_f,B);
    
    double phi2B2 = phi_f*phi_f*B[2];
    
    return (c2_s + phi2B2)/(P_f*V + phi2B2) * Cv_s
           * (phi2B2 + phi_f*B[1])/(phi2B2 + c2_s -Gamma_s*Gamma_s*Cv_s*Ts);
}


Isentrope *EqPorous::isentrope(const HydroState &state)
{
    Isentrope_ODE *S = new Isentrope_ODE(*this, phi_eq->P_min());
    if( S->Initialize(state) )
    {
        EOSerror->Log("EqPorous::isentrope", __FILE__, __LINE__, this,
            "Initialize failed\n" );
        delete S;
        S = NULL;
    }
    return S;
}

int EqPorous::NotInDomain(double V, double e)
{
    double cc = c2(V,e);
    return (!finite(cc) || cc < 0) ? -1 : 0;
}

