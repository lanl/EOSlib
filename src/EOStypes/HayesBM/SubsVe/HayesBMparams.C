#include <EOSbase.h>
#include "HayesBMparams.h"
#include <iostream>
#include <iomanip>
using namespace std;

HayesBMparams::HayesBMparams()
{
    V0 = e0 = P0 = T0 = EOSbase::NaN;
    K0 = Kp0 = G0 = G1 = Cv = EOSbase::NaN;
}

int HayesBMparams::ConvertParams(Convert &convert)
{
	double s_V, s_e, s_T;
	if(  !finite(s_V = convert.factor("V"))
	      || !finite(s_e = convert.factor("e"))
	          || !finite(s_T = convert.factor("T")) )
	{
	    return 1;
	}
	
	V0 *= s_V;
	e0 *= s_e;
	P0 *= s_e/s_V;
	T0 *= s_T;
	
	K0 *= s_e/s_V;
	Cv *= s_e/s_T;
		
	return 0;
}

#define calcvar(var) calc.Variable(#var,&var)
void HayesBMparams::InitParams(Calc &calc)
{
    V0 = e0 = P0 = T0 = EOSbase::NaN;
    K0 = Kp0 = G0 = G1 = Cv = EOSbase::NaN;
	calcvar(V0);
	calcvar(e0);
	calcvar(P0);
	calcvar(T0);
	calcvar(K0);
	calcvar(Kp0);
	calcvar(G0);
	calcvar(G1);
	calcvar(Cv);
    CalcVar *rho0 = new CVpinvdouble(&V0);
    calc.Variable("rho0", rho0);
}

void HayesBMparams::PrintParams(ostream &out) const
{
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\t"   << setw(10) << "V0 = " << V0
	    << "; \t" << setw(10) << "e0 = " << e0 << "\n"
	    << "\t"   << setw(10) << "P0 = " << P0
	    << "; \t" << setw(10) << "T0 = " << T0 << "\n"
	    << "\t"   << setw(10) << "K0 = " << K0
	    << "; \t" << setw(10) << "Kp0 = " << Kp0  << "\n"
	    << "\t"   << setw(10) << "G0 = " << G0
	    << "; \t" << setw(10) << "G1 = " << G1 << "\n"
	    << "\t" << setw(10) << "Cv = " << Cv << "\n";
    
	out.setf(old, ios::adjustfield);	
}

int HayesBMparams::ParamsOK() const
{
    return finite(V0) && finite(e0) && finite(P0) && finite(T0)
           && finite(K0) && finite(Kp0)  && finite(G0) && finite(G1)
           && finite(Cv);
}


double HayesBMparams::P(double V) const
{
    double eta = V0/V;
    double eta2 = pow(eta,(2./3.));
    //double eta5 = eta*eta2;
    //double eta7 = eta5*eta2;
    
    //return P0 + 1.5*K0*(eta7-eta5)*(1.+0.75*(Kp0-4)*(eta2-1));    
    return P0 + 1.5*K0*eta*eta2*(eta2-1)*(1.+0.75*(Kp0-4)*(eta2-1));    
    // eta2 > 1 - (2/3)/(Kp0 -4) for last factor to be positive
    // reasonable domain, 0 < V/V_0 < 1/[1 - (2/3)/(Kp0 -4)]^(3/2)
    //                    and Kp0 > 4 + 2/3
}

double HayesBMparams::dP(double V) const
{
    double eta = V0/V;
    double eta2 = pow(eta,(2./3.));
    //double eta5 = eta*eta2;
    //double eta7 = eta5*eta2;
    //return -(K0/V)*( (3.5*eta7-2.5*eta5)*(1.+0.75*(Kp0-4)*(eta2-1))
    //                  + 0.75*(eta7-eta5)*(Kp0-4)*eta2 );
    return -(K0/V)*eta*eta2*( (3.5*eta2-2.5)*(1.+0.75*(Kp0-4)*(eta2-1))
                      + 0.75*(eta2-1)*(Kp0-4)*eta2 );
}

double HayesBMparams::d2P(double V) const
{
    double eta = V0/V;
    double eta2 = pow(eta,(2./3.));
    //double eta5 = eta*eta2;
    //double eta7 = eta5*eta2;
    //return (K0/V/V)*(
    //   ((35./3.)*eta7-(20./3.)*eta5)*(1.+0.75*(Kp0-4)*(eta2-1))
    //   +((19./4.)*eta7- (15./4.)*eta5)*(Kp0-4)*eta2
    //                );
    return (K0/V/V)*eta*eta2*(
        ((35./3.)*eta2-(20./3.))*(1.+0.75*(Kp0-4)*(eta2-1))
       +((19./4.)*eta2- (15./4.))*(Kp0-4)*eta2
                             );
}

double HayesBMparams::e(double V) const
{
    double eta = V0/V;
    double eta2 = pow(eta,(2./3.));
    double eta4 = eta2*eta2;

    return e0 + P0*(V0 - V)
      + (9./8.)*V0*K0*( eta4-2*eta2+1+0.5*(Kp0-4)*(eta*eta+3*eta2-3*eta4-1) );
}
