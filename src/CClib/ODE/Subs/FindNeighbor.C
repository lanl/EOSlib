#include <cmath>
#include <ODE.h>


int ODE::FindNeighbor(double t)
/* Dummy version which does linear rather than binary search */
{
	if(n_cache == 2)
		return cache = cach1;
		
	if( cache == cach0 )
		cache = NextCache(cache);
	
	double t0 = Cache[cache].t0;
	
	if(t == t0)
		return cache;
	
	if(dir > 0 ? t0 < t : t0 > t)
	{
	//	Search towards cach1
	
		while(cache != cach1)
		{
			cache = NextCache(cache);
			if(dir > 0 ? Cache[cache].t0 >= t :Cache[cache].t0 <= t)
				break;
		}
	}
	else
	{
	//	Search towards cach0
	
		for(int cachp; cache != cach0; cache = cachp)
		{
			cachp = PrevCache(cache);
			if(dir > 0 ? Cache[cachp].t0 <= t : Cache[cachp].t0 >=t)
				break;
		}
	}

	return cache;
}

int ODE::ForwardBracket(double t)
{
	cache = FindNeighbor(t);

	if(dir > 0 && Cache[cache].t0 == t)
		cache = NextCache(cache);
	return cache;
}


int ODE::BackwardBracket(double t)
{
	cache = FindNeighbor(t);
	

	if(dir < 0 && Cache[cache].t0 == t)
		cache = NextCache(cache);
	return cache;
}
