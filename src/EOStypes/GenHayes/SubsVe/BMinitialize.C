#include <EOS.h>
#include "BirchMurnaghan.h"

#define FUNC "BirchMurnaghan::Initialize",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)

int BirchMurnaghan::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    BirchMurnaghanParams::InitParams(calc);
    double V_PTref = 0.0;
    calc.Variable("V_PTref",&V_PTref);
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    
    if( !ParamsOK() )
    {
        EOSerror->Log(FUNC,"variable not set: V0=%lf, e0=%lf\n"
              "\tK0=%lf, Kp0=%lf, a=%lf, b=%lf\n",
              "\tcv=%lf, %lf, %lf, %lf\n",
                V0,e0, K0,Kp0, a,b, cv[0],cv[1],cv[2],cv[3] );
        return 1;
    }

    if( InitODE(BirchMurnaghanParams::eps_abs,BirchMurnaghanParams::eps_rel) )
    {
        EOSerror->Log(FUNC,"InitODE failed\n");
        return 1;
    }
    EOSstatus = EOSlib::good;

    if( std::isnan(V_ref) )
        V_ref = V0;
    if( std::isnan(e_ref) )
    {
         double theta_ref = theta(V_ref);
         double T_ref;
         calc.Fetch("T_ref",T_ref);
         e_ref = ec(V_ref) + theta_ref*IntSCv(T_ref/theta_ref);
    }
    if( V_PTref <= 0.0 )
    {
        P_vac = max(P_vac,Pmin);
        return 0;
    }
    // Adjust V0 such that V_ref = V_PTref at (P_ref,T_ref)
    double T_ref, P_ref;
    calc.Fetch("T_ref",T_ref);
    if( calc.Fetch("P_ref",P_ref) || std::isnan(P_ref) )
    {
         EOSerror->Log(FUNC,"V_PTref > 0 and P_ref = NaN\n");
         return 1;
    }
    V_ref = V_PTref;
    double theta_ref = theta(V_ref);
    double P_max = Pc(V_ref)
                     + Dln_theta(V_ref) * theta_ref*IntSCv(T_ref/theta_ref);
    double V_max = V0;
    double V_min = V0 - (P_max-P_ref)/K0;
    V0 = V_min;
    ParamsOK();
    if( InitODE(BirchMurnaghanParams::eps_abs,BirchMurnaghanParams::eps_rel) )
    {
        EOSerror->Log(FUNC,"InitODE failed in V_PTref iteration\n");
        return 1;
    }
    theta_ref = theta(V_ref);
    double P_min = Pc(V_ref)
                     + Dln_theta(V_ref) * theta_ref*IntSCv(T_ref/theta_ref);
    if( P_min > P_ref || P_max < P_ref )
    {
        EOSerror->Log(FUNC,"InitODE, V_PTref iteration, no bound\n");
        return 1;
    }
    if( abs(P_min-P_max) < 1.e-6* P_ref )
    {
        P_vac = max(P_vac,Pmin);
        return 0;
    }
    int count = 30;
    while( count-- )
    {   // secant iteration
        double x = (P_max-P_ref)/(P_max-P_min);
        if( x < 0. || x > 1. )
            x = 0.5;
        V0 = x*V_min +(1.-x)*V_max;
        ParamsOK();
        if( InitODE(BirchMurnaghanParams::eps_abs,BirchMurnaghanParams::eps_rel) )
        {
            EOSerror->Log(FUNC,"InitODE failed in V_PTref iteration\n");
            return 1;
        }
        theta_ref = theta(V_ref);
        double P0 = Pc(V_ref)
                     + Dln_theta(V_ref) * theta_ref*IntSCv(T_ref/theta_ref);
        if( abs(P0-P_ref) <=  1.e-6* P_ref )
            break;
        if( P0 > P_ref )
        {
            V_max = V0;
            P_max = P0;
        }
        else
        {
            V_min = V0;
            P_min = P0;
        }
    }
    e_ref = ec(V_ref) + theta_ref*IntSCv(T_ref/theta_ref);
    P_vac = max(P_vac,Pmin);
    //P_ref = NaN;
    CalcVar *CV_P = calc.Fetch("P_ref");
    CV_P->set(NaN);
    return 0;
}
