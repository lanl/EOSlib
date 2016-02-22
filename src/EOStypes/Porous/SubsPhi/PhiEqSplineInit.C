#include "PhiEqSpline.h"

#define FUNC "PhiEq_spline::InitPV", __FILE__,__LINE__, this

int  PhiEq_spline::InitPV(double PV_0, double PV_1)
{
	PV_min = PV_0;
	PV_smax = PV_1;
	
	// initialize spline
	int status_p = SplineFunction::init(PV_min,PV_smax, fp(PV_min),NaN);
	if( status_p != 0 )
	{
	    EOSstatus = EOSlib::bad;
	    EOSerror->Log(FUNC, "SplineFunction.init failed with status: %s\n",
	           SplineFunction::ErrorStatus() );
	    return 1;
	}
	
	int N = Nknot();
	
	phi_min = f_k(0);
	phi_smax = f_k(N-1);
	
	// check constraints, phi > PV * d(phi)/d(PV) > 0
	int i;
	double phi[3];
	for(i=0; i<N; i++)
	{
	    double PVi = x_k(i);
	    SplineFunction::Evaluate(PVi,phi);
	    if( phi[1] < 0 || phi[0] < PVi *phi[1])
	    {
	        EOSstatus = EOSlib::bad;
	        EOSerror->Log(FUNC, "phi constraint: %lf %lf %lf %lf %lf\n",
	               PVi, phi[0], phi[1], phi[2], PVi*phi[1] );
		    status_p = 1;
	    }
		
	}
	if( status_p )
	{
	    EOSerror->Log(FUNC, "failed, spline not monotonic\n" );
	    return 1;
	}
	
	// extrapolation
	phi_max = 1;
	PV_max = PV_smax + m*(phi_max-phi[0])/phi[1];

	// initialize energy	
	double dphi = 0.5*(f_k(1)-f_k(0));
	
	double e[1];
	e[0] = 0.0;
	
	int status_e = ODE::Initialize(phi_min, e, dphi);
	if( status_e != 0 )
	{
	    EOSstatus = EOSlib::bad;
	    EOSerror->Log(FUNC, "ODE.Initialize failed with status: %s\n",
	    	   ODE::ErrorStatus(status_e) );
	    return 2;
	}
	
	if( isnan(e_max=Energy(phi_max)) )
	{
	    EOSstatus = EOSlib::bad;
	    EOSerror->Log(FUNC, "Energy(%lf) = NaN\n", phi_max );
	    return 3;	
	}
	
	Espline.rel_tol = 0.001*SplineFunction::rel_tol;
	Espline.abs_tol = Espline.rel_tol*e_max;
	
	double Ep_min = PV(phi_min)/phi_min;
	double Ep_smax = PV(phi_smax)/phi_smax;
	
	status_e = Espline.init(phi_min,phi_smax, Ep_min,Ep_smax);
	if( status_e != 0 )
	{
	    EOSstatus = EOSlib::bad;
	    EOSerror->Log(FUNC, "Espline.init failed with status, %s\n",
	           SplineFunction::ErrorStatus() );
	    return 4;
	}

	// check constraints, dB/dphi > 0 && d^2 B/dphi^2 > 0
	N = Espline.Nknot();
	for(i=0; i<N; i++)
	{
	    double B[3];
	    double phi = Espline.x_k(i);
	    Espline.Evaluate(phi,B);
	    if( B[1] < 0 || B[2] < 0 )
	    {
	        EOSstatus = EOSlib::bad;
	    	EOSerror->Log(FUNC, "energy constraint: %lf %lf %lf %lf\n",
	    	       phi, B[0], B[1], B[2] );
	    	status_p = 1;
	    }
		
	}
	if( status_p )
	{
	    EOSerror->Log(FUNC, "energy constraint failed\n" );
	    return 5;
	}

	return 0;
}
