#include <LocalMath.h>

#include "EOS.h"
#include "Isentrope_ODE.h"

int Isentrope::Initialize(const HydroState &state)
{
    return Wave::Initialize(state);
}

Isentrope::~Isentrope()
{
    // NULL
}

double Isentrope::u_escape(int direction)
{
    WaveState wave;
    int status = P(p_vac, direction, wave);
    return status ? EOS::NaN : wave.u;
}

int Isentrope::u_escape(int direction, WaveState &wave)
{
    return P(p_vac, direction, wave);
}

//

Isentrope_ODE::Isentrope_ODE(EOS &e, double pvac)
            : ODE(2,512), Isentrope(e, pvac)
{
    ODE::epsilon = eos->ODE_epsilon;
}

Isentrope_ODE::~Isentrope_ODE()
{
    // Null
}

double Isentrope_ODE::MaxNorm(const double *y0, const double *y1,
                        const double*, const double*)
{
// Error criterion based only on de/dV = -P
//     du/dV = -rho*c comes along for the ride
// avoids small time steps at initial step if e(0) = 0 or u(0) = 0
    double abs_err = abs(*y0 - *y1);
    double rel_err = abs(*y0 - *y1)/(abs(*y0)+abs(*y1) + epsilon);
    return min(rel_err,abs_err)/epsilon;
}

int Isentrope_ODE::Initialize(const HydroState &state)
{
    if( Isentrope::Initialize(state) )
        return 1;
    
    double y[2];
    y[0] = state.e;
    y[1] = 0.0;        // u-u0
    double dV = -0.01*state.V;
    
    int status = ODE::Initialize(state.V, y, dV);
    if( status )
    {
        eos->ErrorHandler()->Log("Isentrope_ODE::Initialize",
             __FILE__, __LINE__, eos,
             "Isentrope_ODE.Initialize failed with status: %s\n",
             ODE::ErrorStatus(status) );
    }
    return status;
}

// de/dV = -P and du/dV = rho*c
int Isentrope_ODE::F(double *yp, const double *y, double V)
{
    if( V <= 0. )
        return 1;
    double e = y[0];
    // double u = y[1];
    
    double csq = eos->c2(V,e);
    if( isnan(csq) || csq < 0. )
    {
        //eos->ErrorHandler()->Log("Isentrope_ODE::F",
        //     __FILE__, __LINE__, eos, "failed, c2<0\n");
        return 1;
    }
    yp[0] = -eos->P(V,e);
    yp[1] = sqrt(csq)/V;
    return isnan(yp[0]);
}

int Isentrope_ODE::V(double v, int direction, WaveState &wave)
{
    double y[2];
    double yp[2];
    
    int status = Integrate(v, y, yp);
    if( status )
    {
        if( v > state0.V )
            status = (P(p_vac,direction,wave) == 0 ) ? 1 : -1;
        else
        {
            status = -1;
            eos->ErrorHandler()->Log("Isentrope_ODE::V", __FILE__, __LINE__,
                 eos, "Integrate failed with status: %s\n",
                 ODE::ErrorStatus(status));
        }
    }
    else if( -yp[0] < p_vac )
        status = (P(p_vac,direction,wave) == 0 ) ? 1 : -1;
    else
    {
        wave.V = v;
        wave.e = y[0];
        wave.u = state0.u - direction * y[1];

        wave.P = -yp[0];
        wave.us = wave.u + direction*v*yp[1];    // trailing edge
    }
    return status;
}


class P_Isentrope_ODE : public ODEfunc
{
public:
    P_Isentrope_ODE() {}
    double f(double V, const double *y, const double *yp);
};

double P_Isentrope_ODE::f(double, const double *, const double *yp)
{
    return yp[0];
}


int Isentrope_ODE::P(double p, int direction, WaveState &wave)
{
    double V;
    double y[2];
    double yp[2];
    
    double val = -max(p,p_vac);
    P_Isentrope_ODE Ps;
    int status = Integrate(Ps, val, V, y, yp);
    if( status )
    {
        eos->ErrorHandler()->Log("Isentrope_ODE::P", __FILE__, __LINE__, eos,
             "Integrate failed with status: %s\n", ODE::ErrorStatus(status));
        status = -1;
    }
    else
    {
        wave.V = V;
        wave.e = y[0];
        wave.u = state0.u - direction * y[1];
    
        wave.P = -yp[0];
        wave.us = wave.u + direction*V*yp[1];    // trailing edge
        if( p < p_vac )
            status = 1;
    }
    return status;
}


class u_Isentrope_ODE : public ODEfunc
{
public:
    double p_vac;
    double val;
    int stop;
    u_Isentrope_ODE(double p, double v) : p_vac(p), val(v), stop(0) {}
    double f(double V, const double *y, const double *yp);
};

double u_Isentrope_ODE::f(double, const double *y, const double *yp)
{
    if( -yp[0] <= p_vac  && val > y[1])
    {
        stop = 1;
        return val;
    }
    return y[1];
}

int Isentrope_ODE::u(double u, int direction, WaveState &wave)
{
    double V;
    double y[2];
    double yp[2];
    
    double val = direction*(state0.u - u);
    u_Isentrope_ODE u_p(p_vac,val);
    int status = Integrate(u_p, val, V, y, yp);
    if( status )
    {
        status = -1;
        eos->ErrorHandler()->Log("Isentrope_ODE::u", __FILE__, __LINE__, eos,
             "Integrate failed with status: %s\n", ODE::ErrorStatus(status));
    }
    else if( u_p.stop == 1 )
        status =  (P(p_vac, direction, wave)==0) ? 1 : -1; 
    else
    {    
        wave.V = V;
        wave.e = y[0];
        wave.u = state0.u - direction * y[1];
    
        wave.P = -yp[0];
        wave.us = wave.u + direction*V*yp[1];    // trailing edge
    }
    return status;
}

class us_Isentrope_ODE : public ODEfunc
{
public:
    double p_vac;
    double val;
    int stop;
    us_Isentrope_ODE(double p, double v) : p_vac(p), val(v), stop(0) {}
    double f(double V, const double *y, const double *yp);
};

double us_Isentrope_ODE::f(double V, const double *y, const double *yp)
{
    double umc = y[1]-V*yp[1];
    if( -yp[0] <= p_vac  && val > umc )
    {
        stop = 1;
        return val;
    }
    return umc;
}

int Isentrope_ODE::u_s(double us, int direction, WaveState &wave)
{
    double V;
    double y[2];
    double yp[2];
    
    double val = -direction*(us - state0.u);
    us_Isentrope_ODE u_s(p_vac,val);
    int status = Integrate(u_s, val, V, y, yp);
    if( status )
    {
        status = -1;
        eos->ErrorHandler()->Log("Isentrope_ODE::u_s", __FILE__, __LINE__, eos,
             "Integrate failed with status: %s\n", ODE::ErrorStatus(status));
    }
    else if( u_s.stop == 1 )
        status =  (P(p_vac, direction, wave)==0) ? 1 : -1; 
    else
    {
        wave.V = V;
        wave.e = y[0];
        wave.u = state0.u - direction * y[1];
    
        wave.P = -yp[0];
        wave.us = wave.u + direction*V*yp[1];    // trailing edge
    }
    return status;
}

int Isentrope_ODE::Forward(double &V, double *y, double *yp)
{
    return ODE::Forward(V,y,yp,NULL);     // next larger V
}


int Isentrope_ODE::Backward(double &V, double *y, double *yp)
{
    return ODE::Backward(V,y,yp,NULL);     // next smaller V
}

