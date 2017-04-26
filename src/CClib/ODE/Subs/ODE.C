#include <LocalMath.h>
#include <ODE.h>

ODE::ODE(int N_dim, int N_cache) : n_dim(N_dim)
{
	
	if( N_dim < 1)
		exit(-1);
	n_cache = max(2,N_cache);
	CacheStep = (n_cache == 2) ? -1 : n_cache;
	
//	default parameters:

	n_max = 100;
	max_steps = 100000;
	epsilon = 1.0e-10;
	dt_min = 1.0e-10;
	dt_max = HUGE_VAL;
	
	Cache = new CacheState[n_cache];
	
	for(int i = 0; i < n_cache; i++)
		Cache[i].Initialize(n_dim);
		
	Init.Initialize(n_dim);
	
	InftyMinus.t0 = -HUGE_VAL;
	InftyPlus.t0  =  HUGE_VAL;
	
	cach0 = cach1 = -1;
	dir = 0;
	init = 0;
	
	NeverStop = 0;
	IgnoreStop = 0;
	
//	Allocate local storage
	
	dy_full = new double[n_dim];
	 y_full = new double[n_dim];
	
	 dy_half = new double[n_dim];
	  y_half = new double[n_dim];
	y_p_half = new double[n_dim];
	
	dy_2half = new double[n_dim];
	 y_2half = new double[n_dim];
	
	k1 = new double[n_dim];
	k2 = new double[n_dim];
	k3 = new double[n_dim];
	k4 = new double[n_dim];
	
	// set error status if out of storage ?
}

ODE::~ODE()
{
	delete [] Cache;
	
	delete dy_full;
	delete y_full;
	
	delete dy_half;
	delete y_half;
	delete y_p_half;
	
	delete dy_2half;
	delete y_2half;
	
	delete k1;
	delete k2;
	delete k3;
	delete k4;
}



double ODE::MaxNorm(const double *y0, const double *y1,
						const double*, const double*)
{
	double Norm = 0;
	
	for(int i=n_dim; i--; y0++, y1++)
		Norm = max(Norm,
			   std::abs(*y0 - *y1)/((std::abs(*y0)+std::abs(*y1) + 1.)*epsilon));
		//	std::abs(*y0 - *y1)/((std::abs(*y0)+std::abs(*y1) + epsilon)*epsilon));
			
	return Norm;
}
/*********************************************************************
double ODE::MaxNorm(const double *y0, const double *y1,
						const double*, const double*)
{
// ToDo: upgrade ODE.h for relative and std::absolute error
	double Norm = 0;
    double std::abserr = epsilon;
    double relerr = epsilon;
	
	for(int i=Dimension(); i--; y0++, y1++)
    {
        double e1 = std::abs(*y0 - *y1)/((std::abs(*y0)+std::abs(*y1) + relerr)*relerr);
        double e2 = std::abs(*y0 - *y1)/std::abserr;
		Norm = max(Norm, min(e1,e2) );
    }		
	return Norm;
}
********************************************************************/


void ODE::IgnoreStopOff()
{
	IgnoreStop = 0;
	
	ODEstate *state;

	for( Stops.Start(); (state = Stops); Stops++)
	{
		if( t_last < state->t0)
			return;
	}
	
	IgnoreStop = 1;
}


int ODE::GetOuterCacheState(int i, double &t, double *y, double *yp)
{
	ODEstate *s = Initial.Get(i);
	
	if( s == 0 || s->t0 == HUGE_VAL || s->t0 == -HUGE_VAL )
		return EndList;
	
	t = s->t0;
	
	if( y )
		CopyVec(y, s->y0);
	
	if( yp )
		CopyVec(yp, s->y0_prime);
	
	return 0;
}

int ODE::GetStopState(int i, double &t, double *y, double *yp)
{
	ODEstate *s = Stops.Get(i);
	
	if( s == 0 || s->t0 == HUGE_VAL || s->t0 == -HUGE_VAL )
		return EndList;
	
	t = s->t0;
	
	if( y )
		CopyVec(y, s->y0);
	
	if( yp )
		CopyVec(yp, s->y0_prime);
	
	return 0;
}

const char *ODE::ErrorStatus(int status) const
{
	const char *string;
	
	switch(status)
	{
	case 0:
		string = "OK (no error)";
		break;
	case ODE_BUG:
		string = "ODE_BUG";
		break;
	case NOT_INIT:
		string = "NOT_INIT";
		break;
	case STEP_LIMIT:
		string = "STEP_LIMIT";
		break;
	case FAILED:
		string = "FAILED";
		break;
	case NON_CONVERGENCE:
		string = "NON_CONVERGENCE";
		break;
	case STOP_CONDITION:
		string = "STOP_CONDITION";
		break;
	case STOP_ERROR:
		string = "STOP_ERROR";
		break;
	case TIME_STEP_ERROR:
		string = "TIME_STEP_ERROR";
		break;
	case EndList:
		string = "EndList";
		break;
	default:
		string = "unknown error status";
	}
	return string;
}
