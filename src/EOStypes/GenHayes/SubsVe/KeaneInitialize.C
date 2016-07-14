#include <EOS.h>
#include "Keane.h"

#define FUNC "Keane::Initialize",__FILE__,__LINE__,this
#define calcvar(var) calc.Variable(#var,&var)

int Keane::InitParams(Parameters &p, Calc &calc, DataBase *)
{
    KeaneParams::InitParams(calc);
    //double V_PTref = 0.0;
    //calc.Variable("V_PTref",&V_PTref);
    double P0_ref = 0.0;
    calc.Variable("P0_ref",&P0_ref);
    ParamIterator Iter(p);
    char *line = Iter.Parse(calc);
    if( line )
    {
        EOSerror->Log(FUNC,"parse failed for %s\n",line);
        return 1;
    }
    if( !ParamsOK() )
    {
        if( V1 > 0. )
            EOSerror->Log(FUNC,"variable not set: V0=%lf, e0=%lf\n"
              "\tK0=%lf, Kp0=%lf, Kpinfty=%lf\n"
              "\tV1=%lf, Kp1=%lf, Kp1infty=%lf\n"
              "\ta=%lf, b=%lf\n"
              "\tcv=%lf, %lf, %lf, %lf\n",
                V0, e0,
                K0, Kp0, Kp0infty,
                V1, Kp1, Kp1infty,
                a,b, cv[0],cv[1],cv[2],cv[3]
             );
        else
            EOSerror->Log(FUNC,"variable not set: V0=%lf, e0=%lf\n"
              "\tK0=%lf, Kp0=%lf, Kp0infty=%lf\n"
              "\ta=%lf, b=%lf\n"
              "\tcv=%lf, %lf, %lf, %lf\n",
                V0, e0,
                K0, Kp0, Kp0infty,
                a,b, cv[0],cv[1],cv[2],cv[3]
              );
        return 1;
    }

    if( InitODE(KeaneParams::eps_abs,KeaneParams::eps_rel) )
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
    //
    if( P0_ref > 0.0 )
    {
        double T_ref;
        calc.Fetch("T_ref",T_ref);
        double theta_ref = theta(V_ref);
        e_ref = theta_ref*IntSCv(T_ref/theta_ref);
        double P_ref = Pc(V_ref) + Dln_theta(V_ref) * e_ref;
        Pmin -= P0;
        emin -= P0*(V0-Vmax);
        P0 += P0_ref - P_ref;
        e_ref += ec(V_ref);
        Pmin += P0;
        emin += P0*(V0-Vmax);
        CalcVar *CV_P = calc.Fetch("P_ref");
        CV_P->set(NaN);
    }
    if( V1 > 0. )
        set_ePK1();
    P_vac = max(P_vac,Pmin);
    return 0;    
/****************************************************************
*****************************************************************
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
    double Pmax = Pc(V_ref)
                     + Dln_theta(V_ref) * theta_ref*IntSCv(T_ref/theta_ref);
    double Vmax = V0;
    double Vmin,Pmin;
    if( Pmax > P_ref )
    {
        //Vmin = max(V0 - (Pmax-P_ref)/K0, V_ref*(V0/KeaneParams::Vmax) );
        Vmin = V_ref*(V0/KeaneParams::Vmax);
        V0 = Vmin;
        ParamsOK();
        if( InitODE(KeaneParams::eps_abs,KeaneParams::eps_rel) )
        {
            EOSerror->Log(FUNC,"InitODE failed in V_PTref iteration\n");
            return 1;
        }
        theta_ref = theta(V_ref);
        Pmin = Pc(V_ref)
                     + Dln_theta(V_ref) * theta_ref*IntSCv(T_ref/theta_ref);
        if( Pmin > P_ref )
        {
// debug
//cerr << "Pc, Pthermal: " << Pc(V_ref) << " "
//                << theta_ref*IntSCv(T_ref/theta_ref)*Dln_theta(V_ref) << "\n";
            EOSerror->Log(FUNC,"InitODE, V_PTref iteration, no bound\n");
            return 1;
        }
    }
    else
    {
        Pmin = Pmax;
        Vmin = Vmax;
        Vmax = V0 - (Pmin-P_ref)/K0;
        V0 = Vmax;
        ParamsOK();
        if( InitODE(KeaneParams::eps_abs,KeaneParams::eps_rel) )
        {
            EOSerror->Log(FUNC,"InitODE failed in V_PTref iteration\n");
            return 1;
        }
        theta_ref = theta(V_ref);
        Pmax = Pc(V_ref)
                     + Dln_theta(V_ref) * theta_ref*IntSCv(T_ref/theta_ref);
        if( Pmax < P_ref )
        {
            EOSerror->Log(FUNC,"InitODE, V_PTref iteration, no bound\n");
            return 1;
        }
    }
    if( Pmax-Pmin < 1.e-6* P_ref )
    {
        P_vac = max(P_vac,Pmin);
        set_ePK1();
        return 0;
    }
    int count = 30;
    while( count-- )
    {   // secant iteration
        double x = (Pmax-P_ref)/(Pmax-Pmin);
        if( x < 0.1 )
            x = 0.2;
        else if( 0.9 < x )
            x = 0.8;
        V0 = x*Vmin +(1.-x)*Vmax;
        ParamsOK();
        if( InitODE(KeaneParams::eps_abs,KeaneParams::eps_rel) )
        {
            EOSerror->Log(FUNC,"InitODE failed in V_PTref iteration\n");
            return 1;
        }
        theta_ref = theta(V_ref);
        double P0 = Pc(V_ref)
                     + Dln_theta(V_ref) * theta_ref*IntSCv(T_ref/theta_ref);
        if( std::abs(P0-P_ref) <=  1.e-6* P_ref )
            break;
        if( P0 > P_ref )
        {
            Vmax = V0;
            Pmax = P0;
        }
        else
        {
            Vmin = V0;
            Pmin = P0;
        }
    }
    e_ref = ec(V_ref) + theta_ref*IntSCv(T_ref/theta_ref);
    P_vac = max(P_vac,Pmin);
    //P_ref = NaN;
    CalcVar *CV_P = calc.Fetch("P_ref");
    CV_P->set(NaN);
    set_ePK1();
    return 0;
***********************************************************
**********************************************************/
}
