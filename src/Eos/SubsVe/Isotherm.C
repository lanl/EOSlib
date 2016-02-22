#include <LocalMath.h>

#include <EOS.h>
#include <Isotherm_ODE.h>


Isotherm::Isotherm(EOS &e, double pvac) : eos(e.Duplicate()), p_vac(pvac)
{
    if( isnan(p_vac) )
        p_vac = eos->P_vac;
}

Isotherm::~Isotherm()
{
    deleteEOS(eos);
}   

int Isotherm::Initialize(const HydroState &state)
{
    return eos->Evaluate(state,state0);
}

//  

Isotherm_ODE::Isotherm_ODE(EOS &e, double pvac) : ODE(1,512), Isotherm(e, pvac)
{
    ODE::epsilon = eos->ODE_epsilon;
}

Isotherm_ODE::~Isotherm_ODE()
{
    // NULL
}

int Isotherm_ODE::Initialize(const HydroState &state)
{
    if( Isotherm::Initialize(state) )
        return 1;
    
    double y[1];
    y[0] = state.e;
    double dV = -0.01*state.V;
    
    int status = ODE::Initialize(state.V, y, dV);
    if( status )
        eos->ErrorHandler()->Log("Isotherm_ODE::Initialize",
         __FILE__, __LINE__, eos,
             "failed with status: %s\n", ODE::ErrorStatus(status) );
    return status;
}

int Isotherm_ODE::F(double *yp, const double *y, double V)
{
    if( V <= 0 )
        return 1;
    double e = y[0];
    
    double p = eos->P(V,e);
    //double T = eos->T(V,e);
    double T = state0.T;
    double Gamma = eos->Gamma(V,e);
    double Cv = eos->CV(V,e);
    
    yp[0] = -(p-Gamma*Cv*T/V);
    return isnan(yp[0]) || p < 0.9999*p_vac;
}

int Isotherm_ODE::V(double v, ThermalState &state)
{
    double y[1];
    double yp[1];
    
    int status = Integrate(v, y, yp);
    if( status )
    {
        if( v > state0.V )
            status = (P(p_vac,state) == 0 ) ? 1 : -1;
        else
        {
            status = -1;
            eos->ErrorHandler()->Log("Isotherm_ODE::V", __FILE__, __LINE__,
                 eos, "Integrate failed with status: %s\n",
                 ODE::ErrorStatus(status) );
        }
    }
    else
    {
        state.P = eos->P(v,y[0]);
        if( state.P < p_vac )
            status = (P(p_vac,state) == 0 ) ? 1 : -1;
        else
        {            
            state.V = v;
            state.e = y[0];
            state.T = eos->T(v,y[0]);
        }
    }
    return status;
}


class P_Isotherm_ODE : public ODEfunc
{
private:
    EOS *eos;
public:
    P_Isotherm_ODE(EOS *e) : eos(e) {}
    ~P_Isotherm_ODE() {}
    
    double f(double V, const double *y, const double *yp);
};

double P_Isotherm_ODE::f(double V, const double *y, const double *)
{
    return -eos->P(V,y[0]); // -P monotonically increasing with V
}
    

int Isotherm_ODE::P(double p, ThermalState &state)
{   
    double V;
    double y[1];
    double yp[1];
    
    double val = -max(p,p_vac);
    P_Isotherm_ODE PT(eos);
    int status = Integrate(PT, val, V, y, yp);
    if( status )
    {
        status = -1;
        eos->ErrorHandler()->Log("Isotherm_ODE::P", __FILE__, __LINE__, eos,
             "Integrate failed with status: %s\n",
             ODE::ErrorStatus(status) );
    }
    else
    {
        state.V = V;
        state.e = y[0];
    
        state.P = eos->P(V,y[0]);
        state.T = eos->T(V,y[0]);
        if( p < p_vac )
            status = 1;
    }
    return status;
}
