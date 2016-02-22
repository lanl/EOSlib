#include <LocalMath.h>
#include <ODE.h>


int ODE::Step(double t_request)
{
	int max_itt = n_max;
	double h2;
	const double SQRT_2 = sqrt(2.);
	
	CacheState& C1 = Cache[cach1];
	double *y = C1.y0;
	double *y_p = C1.y0_prime;
	double h = C1.h;
	double t = C1.t0;
	
	if( abs(C1.h) < dt_min)
		return TIME_STEP_ERROR;

	int status = 1;
    for( ; max_itt>0; ) // fix rsm,12/8/03
    {
    	while(--max_itt && RungeKuttaStep(y, y_p, dy_full, t, h))
    		h *= 0.5;		
    	if(!max_itt)
		    return NON_CONVERGENCE;
    	AddVec(y_full, y, dy_full);	
    	do
    	{
    		h2 = h/2.0;
    		if(RungeKuttaStep(y, y_p, dy_half, t, h2))
            {
                h = h2;
    			break; //return FAILED;
            }
    		AddVec(y_half, y, dy_half);
    		F(y_p_half, y_half, t + h2);
    		if(RungeKuttaStep(y_half, y_p_half, dy_2half, t+h2, h2))
    			break; //return FAILED;
    			
    		AddVec(dy_2half, dy_half, dy_2half);
    		AddVec(y_2half, y, dy_2half);
    		
    		norm = MaxNorm(y_full, y_2half, dy_full, dy_2half);
            if( !finite(norm) )
                break; //return FAILED;
            if( norm < 1.0)
            {
                status = 0;
    			break;
            }			
    		h = h2;
    		SwapVec(dy_full, dy_half);
    		SwapVec( y_full,  y_half);
    			
    	} while(--max_itt);
        if( status == 0 )
            break;
        h *= 0.5;
    }			
	if(!max_itt)
		return NON_CONVERGENCE;

	int cachp = NextCache(cach1);
	if(cach0 == cachp)
		cach0 = NextCache(cach0);
		
	CacheState& Cnew = Cache[cachp];
	
	C1.h = h;
	
	Cnew.t0 = C1.t0 + h;
	h = (norm < 0.25) ? h * SQRT_2 : h;
	Cnew.h = (h > 0) ? min(dt_max, h) : max(-dt_max, h);
	
	RichardsonInterp(C1.dy, dy_2half, dy_full);
	AddVec(Cnew.y0, y, C1.dy);
	
	if(status =  F(Cnew.y0_prime, Cnew.y0, Cnew.t0))
		return FAILED;
	
	C1.InitInterp(n_dim, dy_half, Cnew.y0_prime);
	cach1 = cachp;

	if( n_steps >= 0 )
	{
		n_steps++;
		
		if( !NeverStop )
		{	
			Cnew.stop_condition =
				StopCondition(Cnew.y0, Cnew.y0_prime, Cnew.t0);
            if( !finite(Cnew.stop_condition) )
                return FAILED;
		}
	
		if( CacheStep > 0 && n_steps >= CacheStep )
		{
			n_steps = 0;
			ODEstate *temp = new ODEstate(n_dim);
			CopyState(*temp, Cnew);
			if( dir > 0 )
			{
				Initial.End();
				Initial.Insert(temp,0);
				t_max = Cnew.t0;
			}
			else
			{
				Initial.Start();
				Initial.Add(temp,0);
				t_min = Cnew.t0;
			}
			Initial.NotPermanent();
		}
	
		if( !NeverStop && ( Cnew.stop_condition == 0 ||
			                ((Cnew.stop_condition > 0) ? C1.stop_condition < 0
                                                       : C1.stop_condition > 0)
			              ) )
		{
			double tau;
			if( Cnew.stop_condition == 0 )
				tau = Cnew.t0;
			else if( StopTime(C1, Cnew, tau) )
				return STOP_ERROR;
				
			ODEstate *temp = new ODEstate(n_dim);
			
			temp->h  = h;
			temp->t0 = tau;
			temp->stop_condition = 0;
			
			Interp(C1, Cnew, (tau - C1.t0)/C1.h, temp->y0, temp->y0_prime);
								
			if( dir > 0 )
			{
				Stops.End();
				Stops.Insert(temp, 0);
			}
			else
			{
				Stops.Start();
				Stops.Add(temp, 0);
			}
			Stops.NotPermanent();
			
			if( !IgnoreStop && (dir > 0 ? t_request >= tau : t_request <= tau) )
			{
				return STOP_CONDITION;
			}
			if(t_request > tau)
				Stops++;
		}
	}
	
	return 0;
}


void ODE::RichardsonInterp(double *dy, double *Dy_2half, double *Dy_full)
{
	// dy = Dy_2half + 1/15 (Dy_2half - Dy_full) + O(h^6)

	for(int i = n_dim; i--; Dy_2half++)
		*dy++ =  *Dy_2half + (1.0/15.0)*(*Dy_2half - *Dy_full++);
}
