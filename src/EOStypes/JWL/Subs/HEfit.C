#include "EOS.h"
#include "HEfit.h"

#include <string>

const char *HEfitlib_vers  = HEfit_vers;
const char *HEfitlib_date  = HEfit_date;

extern "C" { // dynamic link for database
  char *EOS_HEfit_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_HEfit_Info()
  {
      std::string msg(HEfitlib_vers);
      msg = msg + " : " + HEfitlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_HEfit_Constructor()
  {
    EOS *eos = new HEfit;
	return static_cast<void *>(eos);
  }
}

HEfit::HEfit() : EOS("HEfit"), ODE(1,512)
{
    nfCJ = ngCJ = -1;
}

HEfit::~HEfit()
{
	// Null
}


double HEfit::sum_g(double x)
{
    int i = ngCJ;
    double sum = gCJ[i];
    while( i-- )
        sum = gCJ[i] + x*sum;
    return sum;
}

double HEfit::dsum_g(double x)
{
    int i = ngCJ;
    double sum = i*gCJ[i];
    while( --i )
        sum = i*gCJ[i] + x*sum;
    return sum;
}

double HEfit::sum_f(double x)
{
    int i = nfCJ;
    double sum = fCJ[i];
    while( i-- )
        sum = fCJ[i] + x*sum;
    return sum;
}

double HEfit::dsum_f(double x)
{
    int i = nfCJ;
    double sum = i*fCJ[i];
    while( --i )
        sum = i*fCJ[i] + x*sum;
    return sum;
}

double HEfit::Pcj(double rho)
{
    return pow(rho,Gamma0)*sum_f(rho-rho_CJ);
}


double HEfit::dPdrho(double rho)
{
    double x = rho - rho_CJ;
    return pow(rho,Gamma0)*(Gamma0*sum_f(x)/rho + dsum_f(x));
}

int HEfit::F(double *y_prime, const double *, double rho)
{
    y_prime[0] = Pcj(rho)/sqr(rho);
    return 0;
}
double HEfit::Ecj(double rho)
{
    if( rho < rho_min || rho_max < rho )
        return NaN;
    
    double y[1];
    double yp[1];
    
    int status = Integrate(rho, y, yp);
    if( status )
    {
         EOSerror->Log("HEfit::Ecj", __FILE__, __LINE__, this,
              "Integrate failed with status: %s\n", ODE::ErrorStatus(status));
        return NaN;
    }
    return y[0];
}

double HEfit::P(double v, double e)
{
    if( nfCJ < 0 )
        return NaN;
    double rho = 1/v;
    return Pcj(rho) + sum_g(rho-rho_CJ)*(e-Ecj(rho));
}

double HEfit::c2(double v, double e)
{
    double rho = 1/v;
    double x = rho - rho_CJ;
    
    return dPdrho(rho) + (dsum_g(x)+ sqr(v*sum_g(x)))*(e-Ecj(rho));
}

/***
double HEfit::FD(double v, double e)
{
    return NaN; // todo
}
***/

double HEfit::Gamma(double v, double e)
{
    return v * sum_g(1/v - rho_CJ);
}

//


int HEfit::NotInDomain(double v, double e)
{
    if( v < 1/rho_max || rho_min < v )
        return 1;
    double p = P(v,e);
    if( p < Pmin || Pmax < p )
        return 1;
    return 0;
}

// No thermal properties

double HEfit::T(double V, double e)
{
    return NaN;
}

double HEfit::S(double V, double e)
{
    return NaN;
}

double HEfit::CV(double V, double e)
{
    return NaN;
}

Isotherm *HEfit::isotherm(const HydroState &state)
{
    return NULL;
}

int HEfit::PT(double P, double T, HydroState &state)
{
    return 1;
}

