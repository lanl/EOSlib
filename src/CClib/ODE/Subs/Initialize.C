#include <LocalMath.h>
#include <ODE.h>

int ODE::Initialize(double t, double *y_init, double dt)
{
	init = 0;
    if( !finite(t) )
        return FAILED;
	
	n_steps = 0;
	t_min = t_max = t;
	Init.t0 = t;
	CopyVec(Init.y0, y_init);
	int status = F(Init.y0_prime, Init.y0, t);
    if( status )
		return status;
	t_last = t;
 // Outer Cache
    Initial.EmptyList();
	Initial.Add(&InftyMinus);
	Initial.Add(&Init);
	Initial.Append(&InftyPlus);
 // Stops List
    Stops.EmptyList();
	Stops.Add(&InftyMinus);
	if( NeverStop )
    {
		IgnoreStop = 1;
	    Init.stop_condition = 0.;
    }
    else
    {
        Init.stop_condition = StopCondition(Init.y0, Init.y0_prime, t);
	    if( Init.stop_condition == 0 )
		    Stops.Add(&Init);
    }
	Stops.Add(&InftyPlus);
 // time step
	if( dt == 0 )
	{
		Init.h = 0;
		dir = 0;
		status = 0;
	}
	else
	{
	  Init.h = min(dt_max, max(dt_min, std::abs(dt) ) );
		dir = (dt > 0) ? 1 : -1;
		
		cach0 = cach1 = 0;
		CopyState(Cache[0], Init);
		Cache[0].h *= dir;
		
		status = Step(t + Cache[0].h);
		if( status == 0 || status == STOP_CONDITION )
		{
		  Init.h = std::abs(Cache[0].h);
			if( norm < (1./32.) )
			{
                double h = Init.h;
                if( norm < 2.91038e-11 )    // (2^-7)^5
                    Init.h *= 128.;
                else
                    Init.h = ldexp(h,int(0.2*log(1./norm)/log(2.)));
                for( ; Init.h > h; Init.h*=0.5 )
                {
                    status = Step(t + dir*Init.h);
                    if( status==0 )
                        break;
                }
                if( status )
                {
                    status = 0;
                    Init.h = h;
                }
				Stops.EmptyList();
				Stops.Add(&InftyMinus);
				if(!NeverStop && Init.stop_condition == 0)
					Stops.Add(&Init);
				Stops.Add(&InftyPlus);
				
				cach0 = cach1 = 0;
				Cache[0].h = dir * Init.h;
			}
		}
		else
			return status;
		
		cache = cach1;			
		if( status == STOP_CONDITION )
			status = 0;
		if( !IgnoreStop )
			IgnoreStopOff();	// Resynchronize Stops list
	}
	
    if( status == 0 )
	    init = 1;
	return status;
}
