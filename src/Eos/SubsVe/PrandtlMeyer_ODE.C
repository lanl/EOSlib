#include "PrandtlMeyer_ODE.h"

PrandtlMeyer_ODE::PrandtlMeyer_ODE(EOS &e) : PrandtlMeyer(e), ODE(2,512)
{
    // Null
}

PrandtlMeyer_ODE::~PrandtlMeyer_ODE()
{
    // Null
}

int PrandtlMeyer_ODE::F(double *yp, const double *y, double V)
{
    if( V <= 0. )
        return 1;
    double e = y[0];
    // double theta = y[1];   
    double P = eos->P(V,e);
    if( std::isnan(P) || P < 0. )
        return 1;
    double c2 = eos->c2(V,e);
    if( std::isnan(c2) || c2 < 0. )
        return 1;
    double q2 = 2.*(B - e - P*V);
    if( q2 < 0. )
        return 1;
    double M2 = q2/c2;
    if( M2 < 1. )
        return 1;
    yp[0] = -P;
    yp[1] = sqrt(M2-1.)/M2/V;
    return 0;
}

int PrandtlMeyer_ODE::Initialize(PolarState &s)
{
    if( PrandtlMeyer::Initialize(s) )
        return 1;
    double y[2];
    y[0] = state0.e;
    y[1] = 0.0;        // theta-theta0
    double dV = 0.01*state0.V;  
    int status = ODE::Initialize(state0.V, y, dV);
    if( status )
    {
        eos->ErrorHandler()->Log("PrandtlMeyer_ODE::Initialize",
             __FILE__, __LINE__, eos,
             "PrandtlMeyer_ODE.Initialize failed with status: %s\n",
             ODE::ErrorStatus(status) );
    }
    return status;    
}
    
class P_PrandtlMeyer_ODE : public ODEfunc
{
public:
    P_PrandtlMeyer_ODE() {}
    double f(double V, const double *y, const double *yp);
};

double P_PrandtlMeyer_ODE::f(double, const double *, const double *yp)
{
    return yp[0];   // -P
}

int PrandtlMeyer_ODE::P(double p, int dir, PolarWaveState &polar)
{
    double V;
    double y[2];
    double yp[2];
    
    double val = -max(p,eos->P_vac);
    P_PrandtlMeyer_ODE Ps;
    int status = Integrate(Ps, val, V, y, yp);
    if( status )
    {
        status = -1;
        eos->ErrorHandler()->Log("PrandtlMeyer_ODE::P", __FILE__, __LINE__, eos,
             "Integrate failed with status: %s\n", ODE::ErrorStatus(status));
    }
    else
    {
        polar.P = -yp[0];
        polar.V = V;
        polar.e = y[0];
        polar.q = sqrt(2.*(B - polar.e - polar.P*V));
        polar.theta = state0.theta - dir * y[1];
        double c = eos->c(polar.V,polar.e);
        polar.beta = polar.theta + dir*asin(c/polar.q);
        if( p < eos->P_vac )
            status = 1;
    }
    return status;
}

class Theta_PrandtlMeyer_ODE : public ODEfunc
{
public:
    double p_vac;
    double val;
    int stop;
    Theta_PrandtlMeyer_ODE(double p, double dtheta)
        : p_vac(p), val(dtheta), stop(0) {}
    double f(double V, const double *y, const double *yp);
};

double Theta_PrandtlMeyer_ODE::f(double, const double *y, const double *yp)
{
    if( -yp[0] <= p_vac  && val > y[1])
    {
        stop = 1;
        return val;
    }
    return y[1];   // theta
}

int PrandtlMeyer_ODE::Theta(double theta, int dir, PolarWaveState &polar)
{
    double V;
    double y[2];
    double yp[2];
    
    double val = dir*(state0.theta - theta);
    Theta_PrandtlMeyer_ODE theta_p(eos->P_vac,val);
    int status = Integrate(theta_p, val, V, y, yp);
    if( status )
    {
        eos->ErrorHandler()->Log("PrandtlMeyer_ODE::theta", __FILE__, __LINE__, eos,
             "Integrate failed with status: %s\n", ODE::ErrorStatus(status));
        status = -1;
    }
    else if( theta_p.stop == 1 )
        status =  (P(eos->P_vac, dir, polar)==0) ? 1 : -1; 
    else
    {    
        polar.P = -yp[0];
        polar.V = V;
        polar.e = y[0];
        polar.q = sqrt(2.*(B - polar.e - polar.P*V));
        polar.theta = state0.theta - dir * y[1];
        double c = eos->c(polar.V,polar.e);
        polar.beta = polar.theta + dir*asin(c/polar.q);
    }
    return status;
}
