#include <EOSbase.h>
#include "KeaneParams.h"
#include <iostream>
#include <iomanip>
using namespace std;


KeaneParams::KeaneParams()
{
    V0 = EOSbase::NaN;
    e0 = 0.0;
    P0 = 0.0;
    K0 = Kp0 = EOSbase::NaN;
    Kp0infty = EOSbase::NaN;
    V1       = 0.0;
    e1       = 0.0;
    P1       = 0.0;
    K1       = 0.0;
    Kp1      = EOSbase::NaN;
    Kp1infty = EOSbase::NaN;
    a  = b   = EOSbase::NaN;
    theta0 = 1.;
    eps1   = 1.;
    Cv     = 1.;
     cv[0] = cv[1] = cv[2] = cv[3] = EOSbase::NaN;
    eps_abs = 1.;
    eps_rel = 1.e-6;
}

KeaneBase::KeaneBase(const KeaneBase &base)
{
    eos = base.eos;
    *(KeaneParams*)this = base;
}

int KeaneParams::ConvertParams(Convert &convert)
{
	double s_V, s_e, s_T;
	if( !finite(s_V = convert.factor("V"))
	     || !finite(s_e = convert.factor("e"))
	         || !finite(s_T = convert.factor("T")) )
	{
	    return 1;
	}
    V0 *= s_V;
    V1 *= s_V;
    e0 *= s_e;
    e1 *= s_e;
	P0 *= s_e/s_V;
	P1 *= s_e/s_V;
	K0 *= s_e/s_V;
	K1 *= s_e/s_V;
    // Kp0,Kp1           dimensionless
    // Kp0infty,Kp1infty dimensionless
    // a, b              dimensionless
    // cv[4]             dimensionless
    theta0  *= s_T;
    Cv      *= s_e/s_T;
    eps_abs *= s_e/s_T;
    // eps_rel dimmensionless
    Vmax   *= s_V;
    Pmin   *= s_e/s_V;
    emin   *= s_e;
    // eps1 dimensionless
    return 0;
}

#define calcvar(var) calc.Variable(#var,&var)
void KeaneParams::InitParams(Calc &calc)
{
    V0 = EOSbase::NaN;
    e0 = 0.0;
    P0 = 0.0;
    K0 = Kp0 = EOSbase::NaN;
    Kp0infty = EOSbase::NaN;
    V1       = 0.0;
    K1       = 0.0;
    Kp1      = EOSbase::NaN;
    Kp1infty = EOSbase::NaN;

    a = b = EOSbase::NaN;
    theta0  = 1.;
    Cv      = 1.;
    cv[0]   = cv[1] = cv[2] = cv[3] = EOSbase::NaN;
    eps1    = 1.;
    eps_abs = 1;
    eps_rel = 1.e-6;
	calcvar(V0);
	calcvar(e0);
	calcvar(P0);
	calcvar(K0);
	calcvar(Kp0);
	calcvar(Kp0infty);
	calcvar(V1);
	calcvar(Kp1);
	calcvar(Kp1infty);
	calcvar(a);
	calcvar(b);
	calcvar(theta0);
	calcvar(eps1);
    calcvar(eps_abs);
    calcvar(eps_rel);
	calcvar(Cv);
    calc.Array("cv",cv,4);
    CalcVar *rho0 = new CVpinvdouble(&V0);
    calc.Variable("rho0", rho0);    
}

void KeaneParams::PrintParams(ostream &out) const
{
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\t"   << setw(10) << "  V0  = "    << V0
	    << "; \t" << setw(10) << "  e0  = "    << e0
	    << "; \t" << setw(10) << "  P0  = "    << P0      << "\n"
	    << "\t"   << setw(10) << "  K0  = "    << K0
	    << "; \t" << setw(10) << "  Kp0 = "    << Kp0
	    << "; \t" << setw(10) << " Kp0infty = " << Kp0infty << "\n";
    if( V1 > 0.0 )
    {
        out << "\t"   << setw(10) << "  V1  = "    << V1
	        << "; \t" << setw(10) << "  Kp1 = "    << Kp1
	        << "; \t" << setw(10) << " Kp1infty = " << Kp1infty << "\n";
    }
	out << "\t"   << setw(10) << "   a  = "    << a
	    << "; \t" << setw(10) << "   b  = "    << b       << "\n"
	    << "\t"   << setw(10) << " eps1 = "    << eps1
	    << "; \t" << setw(10) << " theta0  = " << theta0  << "\n"
	    << "\t"   << setw(10) << "  Cv  = " << Cv         << "\n"
	    << "\t"   << setw(10) << "cv(1) = " << cv[0]
                                    << ", " << cv[1]
                                    << ", " << cv[2]
                                    << ", " << cv[3]      << "\n"
        << "\t"   << setw(10) << "eps_abs = " << eps_abs
	    << "; \t" << setw(10) << "eps_rel = " << eps_rel  << "\n"
        << "\t#"  << setw(10) << "Vmax    = " << Vmax
	    << "; \t" << setw(10) << "emin    = " << emin     << "\n"
	    << "\t#"  << setw(10) << "Pmin    = " << Pmin     << "\n"
        ;
	out.setf(old, ios::adjustfield);	
}

int KeaneParams::ParamsOK()
{
    int status = finite(V0) && finite(e0)
        && finite(K0) && finite(Kp0) && finite(Kp0infty)
        && finite(a) && finite(b)
        && finite(cv[0]) && finite(cv[1]) && finite(cv[2]) && finite(cv[3]);
    if( !status )
        return 0;
    if( V0 <= 0. )
        return 0;
    if( K0 <= 0. || Kp0 < 1. || Kp0infty <= 1. || Kp0 < Kp0infty )
        return 0;
    double r = Kp0/Kp0infty;
    double x = pow(r/(r-1.),1/Kp0infty);
    Vmax = V0*x;
    Pmin = P0 + K0*( -1./Kp0infty + (r-1.)*log(x) );
    emin = e0 + P0*(V0-Vmax) 
              + V0*K0*( (r/Kp0infty)*( ((r-1.)*x/r -1.)/(Kp0infty-1.) + (x-1.))
                        + (r-1.)*(x*(1.-log(x)) -1. )
                      );
    if( V1 > 0.0 )
    {
        if( V1 >= V0 || Kp1infty <= 1. || Kp1 < Kp1infty )
            return 0;
        set_ePK1();
    }
    return 1;
}

void KeaneParams::set_ePK1()
{
    double eta = V0/V1;
    double r = Kp0/Kp0infty;
    P1 = P0 + K0*( (r/Kp0infty)*(pow(eta,Kp0infty)-1.) -(r-1.)*log(eta) );
    e1 = e0 + P0*(V0-V1)
            +V0*K0*(
            (r/Kp0infty)*((pow(eta,Kp0infty-1)-1.)/(Kp0infty-1.)-(eta-1.)/eta)
            + (r-1.)*((1.+log(eta))/eta - 1.)
           );
    
    K1 = K0 *(1. +r*( pow(eta,Kp0infty)-1.) );
}
