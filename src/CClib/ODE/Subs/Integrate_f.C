#include <LocalMath.h>
#include <OneDFunction.h>
#include <ODE.h>


int ODE::Integrate(ODEfunc &cl, double &val, double &t, double *y, double *yp)
{
//
// Warning:
//	f(t) = cl.f(t,y(t),y_prime(t)) must be monotonically increasing
//	Should be generalize to allow
//		f() to be either increasing or decreasing
//		and to report error when monotonicity fails
//
	if( !init )
		return NOT_INIT;
    if( !finite(val) )
        return FAILED;
		
	int status = 0;
	if( !IgnoreStop && Stops.Dimension() > 2 )
	{
		Integrate(t_last, k1, k2);
		double last_val = cl.f(t_last,k1,k2);
		
	// It can be assumed that: t_last < Stops.Get()->t0
		ODEstate *temp = Stops.Get();
		double stop_val = temp->EvalF(cl);
		
		if( stop_val <= val )
			status = STOP_CONDITION;
		else if( val < last_val )
		{
			temp = Stops.Prev();
			stop_val = temp->EvalF(cl);
			if( stop_val == last_val )
			{
				temp = Stops.Prev();
				stop_val = temp->EvalF(cl);
			}
			if( val <= stop_val )
				status = STOP_CONDITION;
			else
				Stops++;
		}
		
		if(status == STOP_CONDITION)
		{
			t = t_last = temp->t0;
			Stops++;
			val = stop_val;
			CopyVec(y, temp->y0);
			if(yp)
				CopyVec(yp, temp->y0_prime);
				
			return status;
		}
	}
// Check Cache		
	if( dir != 0 )
	{
        double F0 = Cache[cach0].EvalF(cl);
        double F1 = Cache[cach1].EvalF(cl);
        if( dir > 0 )
        {
            if( F0 <= val && val <= F1 )
		    { // within inner cache
			    cache = FindNeighbor(cl, val);
		    }
            else if( F1 < val )
            {
    		    ODEstate *next = Initial.Get(1);
                if( next == &InftyPlus || val < next->EvalF(cl) )
    		    { // extend range of integration
    			    status = Integrate(cl, val);
    			    cache = cach1;
    		    }
                else
    				dir = 0;	// Abandon inner cache
            }
            else
    		    dir = 0;	// Abandon inner cache            
        }
        else
        {
            if( F1 <= val && val <= F0 )
		    { // within inner cache
			    cache = FindNeighbor(cl, val);
		    }
            else if( val < F1 )
            {
    		    ODEstate *prev = Initial.Get(-1);
                if( prev == &InftyMinus && prev->EvalF(cl) < val )
    		    { // extend range of integration
    			    status = Integrate(cl, val);
    			    cache = cach1;
    		    }
                else
    				dir = 0;	// Abandon inner cache
            }
            else
    		    dir = 0;	// Abandon inner cache            
        }
    }
//	Start a new inner cache	
	if( dir == 0 )
	{
		if( val == Init.EvalF(cl) )
		{	/* Current point is t0 */
			CopyVec(y, Init.y0);
			if( yp )
				CopyVec(yp, Init.y0_prime);
			dir = 0;	// Cache is ill-defined
			t = t_last = Init.t0;
			return 0;
		}
		dir = val - Init.EvalF(cl) > 0 ? 1 : -1;
	// find nearest outer cache point
		double f_init = Initial.Get()->EvalF(cl);
		while( f_init < val && Initial.Get(1) != &InftyPlus )
			f_init = Initial.Next()->EvalF(cl);
		while( f_init > val && Initial.Get(-1) != &InftyMinus )
			f_init = Initial.Prev()->EvalF(cl);
		if( dir < 0 && Initial.Get() != &Init && val != f_init )
			Initial.Next();
						
		ODEstate *restart = Initial.Get();
		if( val == restart->EvalF(cl) )
		{
			dir = 0;	// Cache is ill-defined
			CopyVec(y, restart->y0);
			if( yp )
				CopyVec(yp, restart->y0_prime);
			t = t_last = restart->t0;
			return 0;
		}
		else
		{
			cach0 = cach1 = 0;
			if( dir > 0 ? Initial.Get(1)  == &InftyPlus
					    : Initial.Get(-1) == &InftyMinus )
				n_steps = 0;
			else
				n_steps = -1;
			
			
			if( restart == &Init )
			{
				if( Init.h == 0 )
                {
		  double dF = 0.5*std::abs(val - Init.EvalF(cl));
					Init.h = min( max(dF, dt_min), dt_max );
                }
				CopyState(Cache[0], Init);
				Cache[0].h *= dir;
			}
			else
				CopyState(Cache[0], *restart);
			
			status = Integrate(cl, val);				
			cache = cach1;
		}
	}
	
	if( status == 0 )
	{
		status = Interpolate(cl, val, cache, t, y, yp);
		t_last = t;
	}
	else if( status == STOP_CONDITION )
	{
		ODEstate *temp = Stops.Get();
		double stop_val = temp->EvalF(cl);
		
		//if( val < stop_val )
		if( dir > 0 ? val < stop_val : val > stop_val )
		{
			status = Interpolate(cl, val, cache, t, y, yp);
			if( status != 0 )
				return status;
		}
		else
		{
			t = temp->t0;
		}
		
		if( dir > 0 ? t >= temp->t0 : t <= temp->t0 )
		{
			val = cl.f(temp->t0, temp->y0, temp->y0_prime);
			CopyVec(y, temp->y0);
			if(yp)
				CopyVec(yp, temp->y0_prime);
			t_last = t = temp->t0;
			Stops++;
		}
		else
		{
			status = 0;
			if( dir < 0 )
			{
				Stops++;
			}
		}
	}
	return status;
}


int ODE::Integrate(ODEfunc &cl, double val)
{
	int status = ODE_BUG;
	int steps = max_steps;
	
	double t;
	
    double cl0 = Cache[cach1].EvalF(cl);
	if( dir > 0 )
	{
		t = HUGE_VAL;	// dummy time to force STOP_CONDITION
		while( cl0 < val && --steps )
		{
			if( (status=Step(t)) )
				break;
            double cl1 = Cache[cach1].EvalF(cl);
            if( cl1 < cl0 )
                return FAILED;  // cl not monotonic
            cl0 = cl1;
		}
	}
	else
	{
		t = -HUGE_VAL;	// dummy time to force STOP_CONDITION
		while( cl0 > val && --steps )
		{
			if( (status=Step(t)) )
				break;
            double cl1 = Cache[cach1].EvalF(cl);
            if( cl1 > cl0 )
                return FAILED;  // cl not monotonic
            cl0 = cl1;
		}
	}
	
	return steps ? status : STEP_LIMIT;
}

int ODE::FindNeighbor(ODEfunc &cl, double val)
// Dummy version which does linear rather than binary search
// Replace with zeroI()
{
	if( n_cache == 2 )
		return cache = cach1;		
	if( cache == cach0 )
		cache = NextCache(cache);
	
	double f0 = Cache[cache].EvalF(cl);	
	if( val == f0 )
		return cache;
	
	if( dir > 0 ? f0 < val : f0 > val )
	{ // Search towards cach1
		while( cache != cach1 )
		{
			cache = NextCache(cache);
			if( dir > 0 ? Cache[cache].EvalF(cl) >= val
                        : Cache[cache].EvalF(cl) <= val )
				break;
		}
	}
	else
	{ // Search towards cach0
		for( int cachp; cache != cach0; cache = cachp )
		{
			cachp = PrevCache(cache);
			if( dir > 0 ? Cache[cachp].EvalF(cl) <= val
                        : Cache[cachp].EvalF(cl) >= val )
				break;
		}
	}
	return cache;
}

class ODEstopF :  public OneDFunction
{
	friend class ODE;
private:
	ODE *ode;
	ODEfunc &monotone_f;
	
	const CacheState &cach0;
	const CacheState &cach1;
	
	double t0;
	double h;
	
	double *y, *yp;
	
	ODEstopF(ODE *Ode, ODEfunc &cl, const CacheState &Cach0,
							const CacheState &Cach1)
			: ode(Ode), monotone_f(cl),cach0(Cach0), cach1(Cach1)
	{
        rel_tolerance = cl.rel_tol;
        abs_tolerance = cl.abs_tol;

		t0 = Cach0.T0();
		h = Cach0.dt();
		
		y  = ode->k1;	// temporary storage
		yp = ode->k2;	// temporary storage
	}
	~ODEstopF() {}
	double f(double p);
};

double ODEstopF::f(double p)
{
	ode->Interp(cach0, cach1, p, y, yp);
	return monotone_f.f(t0 + p*h, y, yp);
}


int ODE::Interpolate(ODEfunc &cl, double val, int cach,
					double& t, double* y, double *yp)
{
	const CacheState &C0 = Cache[PrevCache(cach)];
	const CacheState &C1 = Cache[cach];
	
	ODEstopF stop(this, cl, C0, C1);
	double p = stop.inverse(val, 0, 1);
	if( stop.Status() == OK )
	{
		t = C0.t0 + C0.h * p;
		Interp(C0, C1, p, y, yp);	
		return 0;
	}
	return STOP_ERROR;
}

