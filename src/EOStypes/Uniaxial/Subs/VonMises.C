#include <EOS.h>
#include "Uniaxial.h"

#include <string>
#include <cstring>

const char *VonMisesElasticPlasticlib_vers  = VonMisesElasticPlastic_vers;
const char *VonMisesElasticPlasticlib_date  = VonMisesElasticPlastic_date;

extern "C" { // dynamic link for database
  char *EOS_VonMisesElasticPlastic_Init()
  {
      if( EOSlib_mismatch(EOS_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + EOSlib_vers + "), derived(" + EOS_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *EOS_VonMisesElasticPlastic_Info()
  {
      std::string msg(VonMisesElasticPlasticlib_vers);
      msg = msg + " : " + VonMisesElasticPlasticlib_date
                + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *EOS_VonMisesElasticPlastic_Constructor()
  {
      EOS *eos = new VonMisesElasticPlastic;
	  return static_cast<void *>(eos);
  }
}

VonMisesElasticPlastic::VonMisesElasticPlastic()
			           : UniaxialEOS("VonMisesElasticPlastic"), G(0), Y(0)
{
	// Null
}
		
VonMisesElasticPlastic::~VonMisesElasticPlastic()
{
	// Null
}

//
// elastic energy = G*||eps_dev||^2 = (2/3)*G*eps_el^2
// no entropy from shear
// above yield, linear energy adjustment needed to account for pressure offset
// 	that is, de = -(P+2/3*Y)dV + TdS
// assumes small strain at yield 0.5*Y/G
//
double VonMisesElasticPlastic::ShearEnergy(double V, double)
{
	if( V <= Vy_comp )
		return V_ref*Y*Y/(6*G)+ (2./3.)*Y*(Vy_comp-V);
	if( V >= Vy_exp )
		return V_ref*Y*Y/(6*G)+ (2./3.)*Y*(V-Vy_exp);
		
	double eps = log(V_ref/V);
	return (2./3.)*V_ref*G*sqr(eps);
}


double VonMisesElasticPlastic::stress_dev(double V, double)
{
	if( V <= Vy_comp )
		return (2./3.)*Y;
	if( V >= Vy_exp )
		return -(2./3.)*Y;
		
	return (4./3.)*G*log(V_ref/V);
}

int VonMisesElasticPlastic::IsElastic(double V, double)
{
	return Vy_comp <= V && V <= Vy_exp;
}


double VonMisesElasticPlastic::Yield(double, double)
{
	return Y;
}

double VonMisesElasticPlastic::ShearModulus(double, double)
{
	return G;
}


double VonMisesElasticPlastic::Vy_compression(double, double)
{
	return Vy_comp;
}

double VonMisesElasticPlastic::Vy_expansion(double, double)
{
	return Vy_exp;
}
