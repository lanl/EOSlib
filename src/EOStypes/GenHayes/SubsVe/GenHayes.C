#include <LocalMath.h>

#include <EOS.h>
#include "GenHayes.h"


GenHayes::GenHayes(const char *eos_type) : EOS(eos_type)
{
	// Null
}

GenHayes::~GenHayes()
{
	// Null
}

double GenHayes::P(double V, double e)
{
    if( V <= 0. )
        return NaN;
    double de = e - ec(V);
    return de < 0. ? NaN : Pc(V) + Dln_theta(V)*de;
}

double GenHayes::e_PV(double p, double V)
{
    if( V <= 0. )
        return NaN;
    double dP = p - Pc(V);
    return (dP < 0.) ? NaN : ec(V)  + dP/Dln_theta(V);
}

double GenHayes::Gamma(double V, double)
{
    return V <= 0. ? NaN : V*Dln_theta(V);
}

double GenHayes::T(double V, double e)
{
    double ecV = ec(V);
    if( V <= 0. ||  e -ecV < -1.e-10*abs(ecV) )
        return NaN;
    if( e -ecV <= 1.e-10*abs(ecV) )
        return 0;
    double theta_V = theta(V);
    return theta_V*InvIntSCv( (e-ecV)/theta_V );
}

double GenHayes::CV(double V, double e)
{
    return SCv( T(V,e)/theta(V) );
}

double GenHayes::S(double V, double e)
{
    return IntSCvT( T(V,e)/theta(V) );
}

double GenHayes::c2(double V, double e)
{
   double ecV = ec(V);
   if( V <= 0. ||  e -ecV < -1.e-10*abs(ecV) )
       return NaN;
   if( e -ecV <= 1.e-10*abs(ecV) )
       return -sqr(V)*dPc(V);
   return sqr(V)*(D2theta(V)*(e-ecV)-dPc(V));
}

int GenHayes::NotInDomain(double V, double e)
{
    double csq = c2(V,e);
    return std::isnan(csq) || csq <= 0.;
}
