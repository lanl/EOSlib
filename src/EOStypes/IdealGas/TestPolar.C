#include <Arg.h>

#include <cmath>
#include <EOS.h>
#include "Isentrope_ODE.h"
#include "Hugoniot_gen.h"
#include "IdealGas.h"

#include <ShockPolar_gen.h>
#include <PrandtlMeyer_ODE.h>

#include <iostream>
using namespace std;


int main(int, char **argv)
{
	ProgName(*argv);	
    double tol = 1.e-6;     // tolerance for accuracy check

    double gamma = 5./3.;
    double Cv    = 1.;
    double V0    = 1.;
    double P0    = 1.;
    double M0    = 4.;
    double Pmin  = 0.001;   // for test of Prandtl-Meyer fan
    int nsteps = 10;
    while(*++argv)
    {
        GetVar(gamma,gamma);
        GetVar(Cv,Cv);
        GetVar(V0,V0);
        GetVar(P0,P0);
        GetVar(M0,M0);
        GetVar(Pmin,Pmin);
        
        GetVar(nsteps,nsteps);
        GetVar(tol,tol);
        
        ArgError;
    }
    IdealGas eos(gamma,Cv);
    eos.P_vac = 1e-10;
	
    double e0 = P0*V0/(gamma-1.);
    double c0 = eos.c(V0,e0);
    double q0 = M0*c0;
    double theta0 = 0.0;
    PolarState state0(V0,e0,q0,theta0);
    int i;
    //
    V_form.width = 8;
    V_form.precision = 5;
    e_form.width = 8;
    e_form.precision = 5;
    q_form.width = 8;
    q_form.precision = 5;
    theta_form.width = 8;
    theta_form.precision = 5;
    P_form.width = 8;
    P_form.precision = 5;
    //
    cout << "ShockPolar\n";
    ShockPolar_gen Polar(eos);
    if( Polar.Initialize(state0) )
        cerr << Error("Polar.Initialize failed") << Exit;
    cout << "        P0=" << Polar.P0        << "\n";
    cout << "      Pmax=" << Polar.Pmax.P    << "\n";
    cout << " beta0_min=" << Polar.beta0_min << "\n";
    //
    PolarWaveStateLabel(cout << setw(12) << " ") << "\n";
    cout << setw(10) << "dtheta_max" << "  " << Polar.dtheta_max << "\n";
    cout << setw(10) << "     sonic" << "  " << Polar.sonic << "\n";   
    cout << "\n";
    //
    cout << setw(10) << "beta"
         << setw(10) << "theta"
         << setw(10) << "P"
         << "\n";
    for( i=0; i<=nsteps; i++ )
    {
        // exact shock polar
        double beta = Polar.beta0_min
                     + (double(i)/double(nsteps))*(0.5*PI-Polar.beta0_min);
        double theta = atan( 2./tan(beta)*(sqr(M0*sin(beta))-1.)/
                                 (M0*M0*(gamma+cos(2.*beta))+2.) );
        double p = Polar.P0*(1.+2.*gamma/(gamma+1.)*(sqr(M0*sin(beta))-1.));
        //
        PolarWaveState pwave;
        if( Polar.beta0(beta,RIGHT,pwave) )
            cerr << Error("Polar.beta0 failed") << Exit;
        if( abs(theta-pwave.theta) > tol )
            cout << "Polar.beta0 > tol: "
                 << setw(10) << setprecision(5) << theta
                 << setw(10) << setprecision(5) << pwave.theta
                 << "\n";
        //
        if( Polar.P(p,RIGHT,pwave) )
            cerr << Error("Polar.P failed") << Exit;
        if( abs(theta-pwave.theta) > tol )
            cout << "Polar.P > tol: "
                 << setw(10) << setprecision(5) << theta
                 << setw(10) << setprecision(5) << pwave.theta
                 << "\n";
        //
        if( p > Polar.dtheta_max.P )
        {
            if( Polar.ThetaHi(theta,RIGHT,pwave) )
                cerr << Error("Polar.ThetaHi failed") << Exit;
            if( abs(p-pwave.P) > tol*(1.+p) )
                cout << "Polar.ThetaHi > tol: "
                     << setw(10) << setprecision(5) << p
                     << setw(10) << setprecision(5) << pwave.P
                     << "\n";
        }
        else
        {
            if( Polar.ThetaLow(theta,RIGHT,pwave) )
                cerr << Error("Polar.ThetaLow failed") << Exit;
            if( abs(p-pwave.P) > tol*(1+p) )
                cout << "Polar.ThetaLow > tol: "
                     << setw(10) << setprecision(5) << p
                     << setw(10) << setprecision(5) << pwave.P
                     << "\n";
        }
        //
        cout << setw(10) << setprecision(5) << beta
             << setw(10) << setprecision(5) << pwave.theta
             << setw(10) << setprecision(5) << pwave.P
             << "\n";
    }
    //
    cout << "\nPrandtl-Meyer fan\n";
    PrandtlMeyer_ODE PM(eos);
    if( PM.Initialize(state0) )
        cerr << Error("PM.Initialize failed") << Exit;
    double sgg = sqrt((gamma+1.)/(gamma-1.));
    double ggp = (gamma-1.)/(gamma+1.);
    double thetaM0 = sgg*atan(sqrt(ggp*(M0*M0-1.))) - atan(sqrt(M0*M0-1.));
    double c02 = PM.c0*PM.c0;
    cout << setw(10) << "P"
         << setw(10) << "theta"
         << setw(10) << "M"
         << "\n";
    for( i=0; i<=nsteps; i++ )
    {
        // exact expansion state
        double p  = P0 - double(i)/double(nsteps)*(P0-Pmin);
        double c2 = c02*pow(p/P0,(gamma-1.)/gamma);
        double M2 = 2.*(PM.B/c2-1./(gamma-1.));
        double theta = sgg*atan(sqrt(ggp*(M2-1.)))-atan(sqrt(M2-1.)) - thetaM0;
        //
        PolarWaveState pwave;
        if( PM.P(p,LEFT,pwave) )
            cerr << Error("PM.P failed") << Exit;
        double M = pwave.q/eos.c(pwave.V,pwave.e);
        if( abs(theta-pwave.theta) > tol )
            cout << "PM.P > tol: "
                 << setw(10) << setprecision(5) << theta
                 << setw(10) << setprecision(5) << pwave.theta
                 << "\n";
        //
        if( PM.Theta(theta,LEFT,pwave) )
            cerr << Error("PM.Theta failed") << Exit;
        if( abs(p-pwave.P) > tol )
            cout << "PM.Theta > tol: "
                 << setw(10) << setprecision(5) << p
                 << setw(10) << setprecision(5) << pwave.P
                 << "\n";
        //        
        cout << setw(10) << setprecision(5) << p
             << setw(10) << setprecision(5) << pwave.theta
             << setw(10) << setprecision(5) << M
             << "\n";
    }
	return 0;
}
