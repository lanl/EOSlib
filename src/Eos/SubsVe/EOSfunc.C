#include "EOS.h"
#include <cmath>

int EOS::NotInDomain(double V, double e)
{
    if( std::isnan(V) || std::isnan(e) || V <= 0 )
        return 1;
    double csq = c2(V,e);
    return (std::isnan(csq) || csq <= 0.0) ? 1 : 0;
}

double EOS::c2(double V, double e)
{
    double p0 = P(V,e);
    double dV = dV_factor*V;
    
    double p1,p2;

    int count;
    for(count=10; count-->0; dV *= 0.25 )
    { 
        double V1 = V - dV;
        double e1 = e + p0*dV;
        double p1 = P(V1,e1);
        if( p1 <= p0 )
            continue;

        double V2 = V1 - dV;
        double e2 = e1 + p1*dV;
        double p2 = P(V2,e2);    
        if( p2 > p1 )
        {
            double dP = 0.5*(p2-p0);
            double d2P = (p2-2*p1+p0);
            if( std::abs(d2P) < c2_tol*dP )
                return V*V*(dP + d2P)/dV;
        }
    }
    return EOSerror->Log("EOS::c2", __FILE__, __LINE__, this, "failed");
}

// FD = -0.5*(1/(rho*c)^2)*(V*d/dV)(rho*c)^2
double EOS::FD(double V, double e)
{
    double p0 = P(V,e);
    double rc2_0 = c2(V,e)/sqr(V);
    double dV = dV_factor*V;
    
    double p1,p2;
    int count;
    for(count=10; count-->0; dV *= 0.25 )
    { 
        double V1 = V - dV;
        double e1 = e + p0*dV;
        double p1 = P(V1,e1);
        if( p1 <= p0 )
            continue;
        e1 = e + 0.5*(p0+p1)*dV;
        double rc2_1 = c2(V1,e1)/sqr(V1);

        double V2 = V1 - dV;
        double e2 = e1 + p1*dV;
        double p2 = P(V2,e2);    
        if( p2 > p1 )
        {
            e2 = e1 + 0.5*(p1+p2)*dV;
            double rc2_2 = c2(V2,e2)/sqr(V2);
            double drc2 = 0.5*(rc2_2-rc2_0);
            double d2rc2 = (rc2_2-2*rc2_1+rc2_0);
            if( std::abs(d2rc2) < FD_tol*std::abs(drc2) )
                return 0.5*(drc2 + d2rc2)/dV *(V/rc2_0);
        }
    }
    return EOSerror->Log("EOS::FD", __FILE__, __LINE__, this,
                  "failed V=%F, e=%F", V, e);
}

double EOS::Gamma(double V, double e)
{
    double p0=P(V,e);
    double de = de_factor*c2(V,e);
    
    int count;
    for(count=20; count-->0; de *= 0.25 )
    { 
        double e1 = e + de;
        double p1 = P(V,e1);

        double e2 = e1 + de;
        double p2 = P(V,e2);    

        double dP = 0.5*(p2-p0);
        double d2P = (p2-2*p1+p0);
        if( std::abs(d2P) < Gamma_tol*dP )
            return V*(dP - d2P)/de;
    }
    return EOSerror->Log("EOS::Gamma", __FILE__, __LINE__, this, "failed");
}

double EOS::T(double, double)
{
    return EOSerror->Log("EOS::T", __FILE__, __LINE__, this, "undefined");
}

double EOS::S(double, double)
{
    return EOSerror->Log("EOS::S", __FILE__, __LINE__, this, "undefined");
}

double EOS::CV(double V, double e)
{
    double T0 = T(V,e);
    double de = de_factor*c2(V,e);
    
    int count;
    for(count=20; count-->0; de *= 0.25 )
    { 
        double e1 = e + de;
        double T1 = T(V,e1);

        double e2 = e1 + de;
        double T2 = T(V,e2);    

        double dT = 0.5*(T2-T0);
        double d2T = (T2-2*T1+T0);
        if( std::abs(d2T) < CV_tol*dT )
            return de/(dT - d2T);
    }
    return EOSerror->Log("EOS::CV", __FILE__, __LINE__, this, "failed");
}


double EOS::CP(double V, double e)
{
    double csq = c2(V,e);
    double G   = Gamma(V,e);
    double Cv  = CV(V,e);
    return csq*Cv/(csq - G*G*Cv*T(V,e));
}

double EOS::KT(double V, double e)
{
    double G   = Gamma(V,e);
    return (c2(V,e) -  G*G*CV(V,e)*T(V,e))/V;
}

double EOS::beta(double V, double e)
{
    double G   = Gamma(V,e);
    double Cv  = CV(V,e);
    return G*Cv/(c2(V,e)-G*G*Cv*T(V,e));
}


int EOS::Evaluate(const HydroState &state1, WaveState &state2)
{
    (HydroState &) state2 = state1;
    state2.P   =  P(state1.V, state1.e);
    double c_2 = c2(state1.V, state1.e);
    if( std::isnan(c_2) || c_2 < 0 )
        return 1;
    state2.us = sqrt(c_2);
    return 0;
}

int EOS::Evaluate(const HydroState &state1, ThermalState &state2)
{
    state2.V = state1.V;
    state2.e = state1.e;
    state2.P   =  P(state1.V, state1.e);
    state2.T   =  T(state1.V, state1.e);
    return std::isnan(state2.T);
}

int EOS::Evaluate(const HydroState &state1, HydroThermal &state2)
{
    (HydroState &) state2 = state1;
    state2.P   =  P(state1.V, state1.e);
    state2.T   =  T(state1.V, state1.e);
    double c_2 = c2(state1.V, state1.e);
    if( std::isnan(c_2) || c_2 < 0 )
        return 1;
    state2.c = sqrt(c_2);
    return 0;
}

double EOS::e_PV(double p, double V)
{
// minimal error checking for efficiency
// fails when p < P_c(V), where P_c is cold curve
    double e = e_ref;
    if( V < V_ref )
        e += p*(V_ref-V);
    int count = Iter_max;
    double tol =  max( OneDFunc_abs_tol, OneDFunc_rel_tol*p );
    while( --count )
    {
        double p1 = P(V,e);
        if( std::isnan(p1) )
            break;
        if( std::abs(p-p1) < tol )
            return e;
        e += V * (p - p1) / Gamma(V,e);
    }
    return NaN;
}
