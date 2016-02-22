#include "Porous.h"


DissipativePorous::DissipativePorous() 
{
	delete [] type;
	type = Strdup("DissipativePorous");
}

DissipativePorous::~DissipativePorous()
{
	// NULL
}


double DissipativePorous::f(double phi)
{
	double e[3];
	if( phi_eq->energy(phi,e) )
	    return EOSerror->Log("DissipativePorous::f",
	        __FILE__, __LINE__,this, "DissipativePorous, f(phi) failed\n");
	
	phi_f = phi;
	  B_f = e[0];
	  P_f = phi_f*solid->P(phi_f*V_f, e_f);
	return V_f*P_f - phi*e[1];	
}



double DissipativePorous::T(double V, double e)
{
	(void) P(V,e);
	return solid->T(phi_f*V, e);
}

double DissipativePorous::S(double V, double e)
{
	(void) P(V,e);
	return solid->S(phi_f*V, e);
}

double DissipativePorous::c2(double V, double e)
{
	(void) P(V,e);
		
	double c2_s = solid->c2(phi_f*V, e);
	double G = solid->Gamma(phi_f*V, e);
	
	double B[3];
	(void) phi_eq->energy(phi_f,B);
	double B2 = phi_f*phi_f*B[2];
	double PV = P_f*V;
	
	return PV + (B2 + PV)*(c2_s-PV)/(B2 + c2_s-G*PV);
}

double DissipativePorous::Gamma(double V, double e)
{
	(void) P(V,e);
	
	double e_s = e-B_f;
	double V_s = phi_f*V;
	double Gamma_s = solid->Gamma(V_s,e_s);

	/***** continuity requires d^2(B)/d(phi)^2 -> infty as phi -> 1
	if( phi_f == 1 )
		return Gamma_s;
	*****/

	double c2_s = solid->c2(V_s,e_s);
	
	double B[3];
	(void) phi_eq->energy(phi_f,B);
	
	double phi2B2 = phi_f*phi_f*B[2];
	
	return (P_f*V + phi2B2)*Gamma_s/(c2_s + phi2B2 - Gamma_s*P_f*V);
}


double DissipativePorous::CV(double V, double e)
{
	(void) P(V,e);
	
	double e_s = e-B_f;
	double V_s = phi_f*V;
	double Cv_s = solid->CV(V_s,e_s);
	
	
	double c2_s    = solid->c2(V_s,e_s);
	double Gamma_s = solid->Gamma(V_s,e_s);
	double Ts      = solid->T(V_s,e_s);
	
	double B[3];
	(void) phi_eq->energy(phi_f,B);
	
	double phi2B2 = phi_f*phi_f*B[2];
	
	return Cv_s/(1+ (P_f*V-Gamma_s*Cv_s*Ts)*Gamma_s/
					(phi2B2 + c2_s -Gamma_s*P_f*V));
}

