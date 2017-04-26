#include <cmath>
#include <OneDFunction.h>
#include <ODE.h>


class ODEstop :  public OneDFunction
{
	friend class ODE;
private:
	ODE *ode;
	
	const CacheState &cach0;
	const CacheState &cach1;
	
	double t0;
	double h;
	
	double *y, *yp;
	
	ODEstop(ODE *Ode, const CacheState &Cach0, const CacheState &Cach1)
					: ode(Ode), cach0(Cach0), cach1(Cach1)
	{
		t0 = cach0.T0();
		h = cach0.dt();
		
		y  = ode->k1;	// temporary storage
		yp = ode->k2;	// temporary storage
	}
	~ODEstop() {}
	double f(double p);
};


double ODEstop::f(double p)
{
	ode->Interp(cach0, cach1, p, y, yp);
	
	return ode->StopCondition(y, yp, t0+p*h);
}


int ODE::StopTime(const CacheState& C0, const CacheState& C1, double &t)
{
	ODEstop stop(this, C0, C1);
		
	t = C0.t0 + stop.zero(0, C0.stop_condition, 1.,C1.stop_condition)*C0.h;
		
	return (stop.Status() == OK) ? 0 : STOP_ERROR;
}


// defaults virtual function
double ODE::StopCondition(const double*, const double*, double)
{
	return 1;
}
