#include <EOS.h>
#include "Hayes.h"

#define FUNC "Hayes::InitParams",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)
int Hayes::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    HayesParams::InitParams(calc);
    // alternative parameters specifying model
    double c0 = NaN;    // sound speed at ref state
    double s  = NaN;    // du_s/du_p on principal Hugoniot
    calcvar(c0);
    calcvar(s);
    CalcVar *rho0 = new CVpinvdouble(&V0);
    calc.Variable("rho0", rho0);
    
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( !std::isnan(c0) )
    {
        if( std::isnan(Gamma0) || isnan(V0) || isnan(Cv) || isnan(T0) )
        {
            EOSerror->Log(FUNC,"must specify Gamma0, V0, Cv, T0 with c0\n");
            return 1;
        }
        K0 = (c0*c0 - Gamma0*Gamma0*Cv*T0)/V0;
    }
    if( !std::isnan(s) )
    {
        if( std::isnan(Gamma0) || isnan(V0) || isnan(K0) || isnan(Cv) || isnan(T0) )
        {
            EOSerror->Log(FUNC,"must specify Gamma0, V0, K0, Cv, T0 with s\n");
            return 1;
        }
        double R = Gamma0*Cv*T0/(K0*V0);
        N = 4*s*(1 + Gamma0*R) - (1+Gamma0*Gamma0*R);
    }    
    if( !ParamsOK() )
    {
        EOSerror->Log(FUNC,"variable not set: V0=%lf, e0=%lf, P0=%lf, T0=%lf\n"
              "\tK0=%lf, N=%lf, Gamma0=%lf, Cv=%lf\n",
              V0, e0, P0, T0, K0, N, Gamma0, Cv);
        return 1;
    }

    if( std::isnan(V_ref) )
        V_ref = V0;
    if( std::isnan(e_ref) )
        e_ref = e0;
    return 0;
}
