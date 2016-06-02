#include <LocalMath.h>
#include <ODE.h>


int ODE::Integrate(double &t, double *y, double *yp)
{
	if( !init )
		return NOT_INIT;
    if( !finite(t) || t >= HUGE_VAL || t <= -HUGE_VAL )
        return FAILED;
		
	int status = 0;	
	//if( !IgnoreStop && )
	if( !IgnoreStop && Stops.Dimension() > 2 )
	{ // It can be assumed that: t_last < Stops.Get()->t0
		ODEstate *temp = Stops.Get();
	
		if( temp->t0 <= t )
			status = STOP_CONDITION;
		else if( t < t_last )
		{
			temp = Stops.Prev();
			if( temp->t0 == t_last )
				temp = Stops.Prev();
				
			if( t <= temp->t0 )
				status = STOP_CONDITION;
			else
				Stops++;
		}
		
		if( status == STOP_CONDITION )
		{
			t = t_last = temp->t0;
			Stops++;
			
			CopyVec(y, temp->y0);
			if(yp)
				CopyVec(yp, temp->y0_prime);
				
			return status;
		}
	}
// Check Cache		
	if( dir != 0 )
	{
		if( dir > 0 ? (Cache[cach1].t0 < t && t <  Initial.Get(1)->t0)
                    : (Initial.Get(-1)->t0 < t && t <  Cache[cach1].t0) )
		{ // extend range of integration
			status = Integrate(t);
			cache = cach1;
		}
		else if( dir > 0 ? (Cache[cach0].t0 <= t && t <=  Cache[cach1].t0)
                         : (Cache[cach1].t0 <= t && t <=  Cache[cach0].t0) )
		{ // within inner cache
			cache = FindNeighbor(t);
		}
		else if( dir > 0 ? (Initial.Get(1)->t0  <=t && t< Initial.Get(2)->t0)
                         : (Initial.Get(-1)->t0 >=t && t> Initial.Get(-2)->t0) )
		{
			CacheState &C1 = Cache[cach1]; 
			if( std::abs(t-C1.t0) < (n_cache/4)*C1.h )
			{
			// Maintain continuity of inner cache
				n_steps = -1;
				if( dir > 0 ? Initial.Get(2)  == &InftyPlus
                            : Initial.Get(-2) == &InftyMinus )
				{
					Integrate(Initial.Get(dir)->t0);
					n_steps = 0;
				}
				
				if( dir>0 )
					Initial.Next();
				else
					Initial.Prev();
					
				status = Integrate(t);
				cache = cach1;
			}
			else
			{
				dir = 0;	// Abandon inner cache
			}
		}
		else
		{
			dir = 0;		// Abandon inner cache
	
		}
	}
//	Start a new inner cache		
	if( dir == 0 )
	{
		if( t == Init.t0 )
		{	/* Current point is t0 */
			CopyVec(y, Init.y0);
			if( yp )
				CopyVec(yp, Init.y0_prime);
			dir = 0;	// Cache is ill-defined
			t_last = t;
			return 0;
		}
		dir = t - Init.t0 > 0 ? 1 : -1;
		double t_init = Initial.Get()->t0;
		while( t_init < t )
			t_init = Initial.Next()->t0;
		while( t_init > t )
			t_init = Initial.Prev()->t0;
		if( dir < 0 && Initial.Get() != &Init && t != t_init )
			Initial.Next();		
        
		ODEstate *restart = Initial.Get();       
		if( t == restart->t0 )
		{
			dir = 0;	// Cache is ill-defined
			CopyVec(y, restart->y0);
			if( yp )
				CopyVec(yp, restart->y0_prime);
			t_last = t;
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
		  double diff = 0.5 * std::abs(t - Init.t0);
					Init.h = min( max(diff, dt_min),  dt_max );
                }
				CopyState(Cache[0], Init);
				Cache[0].h *= dir;
			}
			else
				CopyState(Cache[0], *restart);
			
			status = Integrate(t);				
			cache = cach1;
		}
	}
	
	if( status == STOP_CONDITION )
	{
		ODEstate *temp = Stops.Get();
		CopyVec(y, temp->y0);
		if( yp )
			CopyVec(yp, temp->y0_prime);
		t_last = t = temp->t0;
		Stops++;
		return status;
	}
	if( !status )
	{
		t_last = t;
		Interpolate(cache, t, y, yp);
	}

	return status;
}





int ODE::Integrate(double t)
{
	int status = ODE_BUG;
	int steps = max_steps;
	
	if( dir > 0 )
	{
		while( Cache[cach1].t0 < t && --steps )
		  if( (status = Step(t)) )
				break;
	}
	else
	{
		while( Cache[cach1].t0 > t && --steps )
		  if( (status = Step(t)) )
				break;
	}
	
	return steps ? status : STEP_LIMIT;
}
