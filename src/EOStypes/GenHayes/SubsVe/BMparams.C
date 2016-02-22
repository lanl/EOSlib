#include <EOSbase.h>
#include "BirchMurnaghanParams.h"
#include <iostream>
#include <iomanip>
using namespace std;


BirchMurnaghanParams::BirchMurnaghanParams()
{
    V0 = e0  = EOSbase::NaN;
    K0 = Kp0 = EOSbase::NaN;
    a  = b   = EOSbase::NaN;
    theta0 = 1.;
    eps1   = 1.;
    Cv     = 1.;
     cv[0] = cv[1] = cv[2] = cv[3] = EOSbase::NaN;
    eps_abs = 1.;
    eps_rel = 1.e-6;
}

BirchMurnaghanBase::BirchMurnaghanBase(const BirchMurnaghanBase &base)
{
    eos = base.eos;
    *(BirchMurnaghanParams*)this = base;
}

int BirchMurnaghanParams::ConvertParams(Convert &convert)
{
	double s_V, s_e, s_T;
	if( !finite(s_V = convert.factor("V"))
	     || !finite(s_e = convert.factor("e"))
	         || !finite(s_T = convert.factor("T")) )
	{
	    return 1;
	}
    V0 *= s_V;
    e0 *= s_e;
	K0 *= s_e/s_V;
    // Kp0   dimensionless
    // a, b  dimensionless
    // cv[4] dimensionless
    theta0  *= s_T;
    Cv      *= s_e/s_T;
    eps_abs *= s_e/s_T;
    // eps_rel dimmensionless
    Vmax   *= s_V;
    Pmin   *= s_e/s_V;
    e1     *= s_e;
    // eps1 dimensionless
    return 0;
}

#define calcvar(var) calc.Variable(#var,&var)
void BirchMurnaghanParams::InitParams(Calc &calc)
{
    V0 = e0  = EOSbase::NaN;
    K0 = Kp0 = EOSbase::NaN;
    a = b = EOSbase::NaN;
    theta0  = 1.;
    Cv      = 1.;
    cv[0]   = cv[1] = cv[2] = cv[3] = EOSbase::NaN;
    eps1    = 1.;
    eps_abs = 1;
    eps_rel = 1.e-6;
	calcvar(V0);
	calcvar(e0);
	calcvar(K0);
	calcvar(Kp0);
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

void BirchMurnaghanParams::PrintParams(ostream &out) const
{
	FmtFlags old = out.setf(ios::right, ios::adjustfield);
	out << "\t"   << setw(10) << "  V0  = "    << V0
	    << "; \t" << setw(10) << "  e0  = "    << e0      << "\n"
	    << "\t"   << setw(10) << "  K0  = "    << K0
	    << "; \t" << setw(10) << "  Kp0 = "    << Kp0     << "\n"
	    << "\t"   << setw(10) << "   a  = "    << a
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
	    << "; \t" << setw(10) << "e1      = " << e1       << "\n"
	    << "\t#"  << setw(10) << "Pmin    = " << Pmin     << "\n"
        ;
	out.setf(old, ios::adjustfield);	
}

int BirchMurnaghanParams::ParamsOK()
{
    int status = finite(V0) && finite(e0)
        && finite(K0) && finite(Kp0) && finite(a) && finite(b)
        && finite(cv[0]) && finite(cv[1]) && finite(cv[2]) && finite(cv[3]);
    if( !status )
        return 0;
    double A = 0.75*(Kp0-4.);
    double x = 3.5+ 2.*A;
    double y = 1. + (-x+sqrt(x*x-18.*A) )/(9.*A);
    double eta = 1./pow(y,-3./2.);
    double eta2 = pow(eta,(2./3.));
    double eta5 = eta*eta2;
    double eta7 = eta5*eta2;
    Pmin = 1.5*K0*(eta7-eta5)*(1.+0.75*(Kp0-4.)*(eta2-1.));
    Vmax = V0/eta;
    double eta4 = eta2*eta2;
    e1 = e0 + (9./8.)*V0*K0*( eta4-2.*eta2+1.
            +0.5*(Kp0-4.)*(eta*eta+3.*eta2-3.*eta4-1.) );
    return 1;
}
