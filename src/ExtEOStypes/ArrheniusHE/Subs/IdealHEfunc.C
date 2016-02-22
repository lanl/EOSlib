#include "IdealHE.h"

double *IdealHE::z_f(double V, double e)
{
    ztmp[0] = frozen ? (*z_ref)(0) : 1;
    return ztmp;        
}

int IdealHE::ddlambda(double V,   double e,  double lambda,
                      double &dP, double &dT)
{
    dP = (gamma-1.)*Q/V;
    dT = Q/Cv;
    return 0;
}

int IdealHE::ddz(int i, double V,   double e,  const double *z,
                            double &dP, double &dT)
{
    if( i != 0 )
        return -1;
    return ddlambda(V,e,z[0], dP,dT) ? -1 : 0;  
}

double IdealHE::P(double V, double e, const double *z)
{
    return (gamma-1.)*(e+z[0]*Q)/V;
}

double IdealHE::T(double V, double e, const double *z)
{
    return (e+z[0]*Q)/Cv;
}

double IdealHE::S(double V, double e, const double *z)
{
    double Pref = (gamma-1.)*e_ref/V_ref;
    double p = (gamma-1.)*(e+z[0]*Q)/V;
    return Cv*(log(p/Pref) + gamma*log(V/V_ref));
}

double IdealHE::c2(double V, double e, const double *z)
{
    return gamma*(gamma-1.)*(e+z[0]*Q);
}

double IdealHE::Gamma(double V, double e, const double *z)
{
    return gamma - 1.;
}

double IdealHE::CV(double V, double e, const double *z)
{
    return Cv;
}

double IdealHE::CP(double V, double e, const double *z)
{
    return gamma*Cv;
}

double IdealHE::KT(double V, double e, const double *z)
{
    return (gamma-1.)*(e+z[0]*Q)/V;
}

double IdealHE::beta(double V, double e, const double *z)
{
    return Cv/(e+z[0]*Q);   // 1/T
}

double IdealHE::FD(double V, double e, const double *z)
{
    return 0.5*(gamma+1.);
}

int IdealHE::NotInDomain(double V, double e, const double *z)
{
    return (V<=0. || e+z[0]*Q<=0.) ? 1 : 0;
}

int IdealHE::PT(double P, double T, HydroState &state)
{
	if( P <= 0 || T <= 0 )
		return 1;
	state.e  = Cv*T;
	state.V  = (gamma-1.)*state.e/P;
	state.e -= lambda_ref()*Q;
	return 0;
}
