#include "PhiTypes.h"

#include <string>
#include <cstring>


extern "C" { // dynamic link for database
  char *PhiEq_Phi1_Init()
  {
      if( PhiEqlib_mismatch(PhiEq_vers) )
      {
        std::string msg("Library incompatiblity: base(");
        msg = msg + PhiEqlib_vers + "), derived(" + PhiEq_vers + ")\n";
        return strdup(msg.c_str());
      }
      return NULL;
  }
  
  char *PhiEq_Phi1_Info()
  {
      std::string msg(PhiEqlib_vers);
      msg = msg + " : " + PhiEqlib_date + " : " + BugReport + "\n";
      return strdup(msg.c_str());
  }

  void *PhiEq_Phi1_Constructor()
  {
      PhiEq *phi = new Phi1;
      return static_cast<void*>(phi);
  }
}

Phi1::Phi1() : PhiEq_spline("Phi1")
{
	abs_tol = 1e-4;
	rel_tol = 1e-4;
	
	V0 = 1/1.9;
	P0 = 1e-4;
	PV0 = P0*V0;
	PV1 = 0.7*V0;
	phi0 = 0.64;
	phi1 = 1;
	//
	n=10;
	a = 1.6;
	dphidP0 = 0.04;
	dphidP1 = 0.2;
	//
	sphi0 = phi0;
	sPV1 = PV1;
}

Phi1::~Phi1()
{
	// Null
}


/**********************
		n=400;
		a = 1.6;
		dphidP0 = 0.05;
		dphidP1 = 0.2;
double Phi1::f(double sPV)
{
	sPV = PV0 + (PV1 - PV0)*(sPV-PV0)/(sPV1-PV0);
	
	double x = log(sPV/PV0)/log(PV1/PV0);
	double sphi =  phi0*(1+2*x)*(1-x)*(1-x)*(1-a*x*x*exp(-n*(sPV-PV0)/PV1))
	        + dphidP0*x*(1-x)*(1-x)  + phi1*x*x*(3-2*x) - dphidP1*x*x*(1-x);

	sphi = sphi0 + (phi1-sphi0)*(sphi - phi0)/(phi1- phi0);	
	return sphi;
}
***********************/

//

double Phi1::f(double sPV)
{
	sPV = PV0 + (PV1 - PV0)*(sPV-PV0)/(sPV1-PV0);
	
	double x = log(sPV/PV0)/log(PV1/PV0);
	double sphi =  phi0*(1+2*x)*(1-x)*(1-x)*(1-a*x*x*exp(-n*x*x))
	        + dphidP0*x*(1-x)*(1-x) + phi1*x*x*(3-2*x) - dphidP1*x*x*(1-x);

	sphi = sphi0 + (phi1-sphi0)*(sphi - phi0)/(phi1- phi0);	
	return sphi;
}

int Phi1::InitPV(double PV_0, double PV_1)  
{
	if( std::isnan(PV_0) )
		PV_0 = PV0;
	if( std::isnan(PV_1) )
		PV_1 = 0.98*sPV1;
	PhiEq::Pmin = P0;
	return PhiEq_spline::InitPV(PV_0, PV_1);
}

