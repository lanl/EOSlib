#include <LocalMath.h>
#include <ODE.h>


int ODE::Forward(double &t, double *y, double *yp, int *bracket)
{
	if(!init)
		return NOT_INIT;
		
	int status = 0;
	
	if( dir != 0 )
	{
		if( dir > 0 ?
			(Cache[cach1].t0 <= t && t <  Initial.Get(1)->t0) :
			(Initial.Get(-1)->t0 < t && t <  Cache[cach1].t0) )
		{
			status = ForwardIntegrate(t);
			cache = cach1;
		}
		else if( dir > 0 ?
			(Cache[cach0].t0 <= t && t <  Cache[cach1].t0) :
			(Cache[cach1].t0 <= t && t <  Cache[cach0].t0)   )
		{
			cache = ForwardBracket(t);
		}
		else if(dir > 0 ?
			(Initial.Get(1)->t0 <=t && t< Initial.Get(2)->t0) :
			(Initial.Get(-1)->t0 >=t && t > Initial.Get(-2)->t0) )
		{
			CacheState &C1 = Cache[cach1]; 
			if( abs(t-C1.t0) < (n_cache/4)*C1.h )
			{
			// Maintain continuity of inner cache
				n_steps = -1;
				if( dir > 0 ? Initial.Get(2) == &InftyPlus :
					Initial.Get(-2) == & InftyMinus )
				{
					ForwardIntegrate(Initial.Get(dir)->t0);
					n_steps = 0;
				}
				
				if(dir>0)
					Initial.Next();
				else
					Initial.Prev();
					
				status = ForwardIntegrate(t);
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
		dir = t - Init.t0 >= 0 ? 1 : -1;
		
		double t_init = Initial.Get()->t0;
		
		while(t_init < t)
			t_init = Initial.Next()->t0;
			
		while(t_init > t)
			t_init = Initial.Prev()->t0;
			
		if(dir < 0)
			Initial.Next();
						
			
		ODEstate *restart = Initial.Get();
			
		cach0 = cach1 = 0;
		
		if( dir > 0 ? Initial.Get(1) == &InftyPlus :
				Initial.Get(-1) == & InftyMinus )
			n_steps = 0;
		else
			n_steps = -1;
		
		
		if(restart == &Init)
		{
			if( Init.h == 0 )
				Init.h = min(max(0.5 * abs(t - Init.t0),
						      dt_min),  dt_max);
				
			CopyState(Cache[0], Init);
			Cache[0].h *= dir;
		}
		else
			CopyState(Cache[0], *restart);
		
		status = ForwardIntegrate(t);				
		cache = cach1;
	}
	
	if(status)
		return status;
		
	int cachp = dir > 0 ? cache : PrevCache(cache);
	
	t_last = t = Cache[cachp].t0;
	
	if(!IgnoreStop)
	{
		while(Stops.Get()->t0 > t)
			Stops.Prev();
		while(Stops.Get()->t0 <= t)
			Stops.Next();
	}
	
	CopyVec(y, Cache[cachp].y0);
	if( yp )
		CopyVec(yp, Cache[cachp].y0_prime);
		
	if( bracket )
	{
		bracket[0] = PrevCache(cache);
		bracket[1] = cache;
	}
	
	
	return 0;
}





int ODE::ForwardIntegrate(double t)
{
	int status = ODE_BUG;
	int steps = max_steps;
	
	int temp = IgnoreStop;
	IgnoreStop = 1;
	
	if(dir > 0)
	{
		while(Cache[cach1].t0 <= t && --steps)
			if(status = Step(t))
				break;
	}
	else
	{
		while(Cache[cach1].t0 > t && --steps)
			if(status = Step(t))
				break;
	}
	
	IgnoreStop = temp;
	
	return steps ? status : STEP_LIMIT;
}
