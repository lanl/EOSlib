#include <LocalMath.h>
#include <OneDFunction.h>
#include <iostream>
#include <limits>
using namespace std;

double OneDFunction::PRE_ZERO(double x_guess)
{
	status = OK;
	
	y0 = f(x0);
	
	if( status || ::isnan(y0) )
	{
		status = ZERO_LOWER_LIMIT;
		return x0;
	}
	
	y1 = f(x1);
	
	if( status || ::isnan(y1) )
	{
		status = ZERO_UPPER_LIMIT;
		return x1;
	}
	
	return PRE_CONDITION(x_guess);
}

double OneDFunction::PRE_CONDITION(double x_guess)
{
// Check for degenerate case
	if( y0 == y_goal )
	{
		x1 = x0;
		y1 = y0;
		return x0;
	}
		
	if( y1 == y_goal )
	{
		x0 = x1;
		y0 = y1;
		return x1;
	}
		
// Check that limits form a bracket
	if( x0 == x1 )
	{
		status = ZERO_NO_BRACKET;
		return x0;
	}
	if( y0 > y_goal ? y1 > y_goal : y1 < y_goal )
	{
		status = ZERO_NO_BRACKET;
		return(y0 > y_goal ? HUGE : -HUGE);
	}
	
// Interchange limits if necessary for y0 < y1
	if( y0 > y1 )
	{
		double temp;
			
		temp = y0;
		y0 = y1;
		y1 = temp;
		
		temp = x0;
		x0 = x1;
		x1 = temp;
	}
	
// Check for valid initial guess		
	if( x0 < x1 ? (x0 < x_guess && x_guess < x1) :
					(x1 < x_guess && x_guess < x0) )
	{
		status = OK;
	
	// evaluate at x_guess
		double y_guess = f(x_guess);
		if( status || ::isnan(y_guess) )
		{
			status = ZERO_INSIDE;
			return x_guess;
		}
		if( y_guess == y_goal )
		{
			x0 = x1 = x_guess;
			y0 = y1 = y_guess;
			return x_guess;
		}

	// Newton's method with unevenly spaced points	
		if( y0 < y_guess && y_guess < y1 )
		{
		// linearly interpolate derivative, same as quadratic fit
			double yp = (x1-x_guess) * (y0-y_guess)/(x0-x_guess)
				+ (x_guess-x0) * (y1-y_guess)/(x1-x_guess);
				
			double dx = (x_guess - x0)/(x1-x0);
			double dx_prime = - 2. * (y_guess - y_goal) / yp;
			
		
		// check trial within bounds	
			if( y_guess > y_goal ? 0 < dx_prime + dx
							: dx + dx_prime < 1 )
		
			// Replace one end point of bracket by (x_guess,y_guess)
				if( y_guess > y_goal )
				{
					x1 = x_guess;
					y1 = y_guess;
				}
				else
				{
					x0 = x_guess;
					y0 = y_guess;
				}
			
	
			// Find best bracket centered on f(xi) = y_goal
				x_guess += dx_prime * (x1-x0);
					
				y_guess = f(x_guess);
				if( status || ::isnan(y_guess) )
				{
					status = ZERO_INSIDE;
					return x_guess;
				}
				if( y_guess == y_goal )
				{
					x0 = x1 = x_guess;
					y0 = y1 = y_guess;
					return x_guess;
				}
		}
		
	// Replace one end point of bracket by (x_guess, y_guess)
		if( y_guess > y_goal )
		{
			y1 = y_guess;
			x1 = x_guess;
		}
		else
		{
			y0 = y_guess;
			x0 = x_guess;
		}
	}
		
	ZERO();
	
	return 0.5*(x0+x1);
}


void OneDFunction::ZERO()
{
//
//	Iteratively improve brackets on zero crossing using
//	the best of either bisection or Newton's methods.
//	Assumes y0 < y_goal < y1
//
	double x, y;
	double dx, dx_prime;
	double dy;
		
	rel_tolerance = max( rel_tolerance, MACH_EPS );
	
	status = OK;
	for( n_cycles = max_cycles; n_cycles--; )
	{
		dx = x1 - x0;
		x = 0.5*(x0 + x1);
		
    /**************************************
	// check abs_tolerance if bracket contains 0, otherwise rel_tolerance	
		if( ( (x0 > 0 && x1 > 0) || (x0 < 0 && x1 < 0) ) ?
		    abs(dx) < rel_tolerance * abs(x) : abs(dx) < abs_tolerance )
		{
			return;
		}
    ***************************************/
        if( abs(dx) < max(abs_tolerance,rel_tolerance*abs(x)) )
            return;

	// Compute function at center point of bracket
		y = f(x);
		if( status || ::isnan(y) )
		{
			status = ZERO_INSIDE;
			return;
		}
		if( y == y_goal )
		{
			x0 = x1 = x;
			y0 = y1 = y;
			return;
		}

	// Try Newton's method if quadratic fit is monotonic
		
		// y(x) = y + a * x - 4 b * x^2
		//	For equally spaced points x0, x, x1 scaled to (-0.5,0.5)
		// 	a = y1 - y0
		// 	b = y - 0.5 * (y1 + y0)
		// monotonic in (-0.5, 0.5) if |b/a| < 0.25

		// check quadratic is monotonic
		dy = y1-y0;		
		if( abs(y - 0.5 * (y1 + y0)) < 0.25*dy )
		{
		// Replace one end point of bracket by (x, y)
		// and find new point x nearest to y_goal
		//
		// Note: Finding x_goal from quadratic rather than
		//	linear fit complicates the algorithm
		//	without improving convergence!
		//
			if( y > y_goal )
			{
				x1 = x;
				if( y-y_goal <= y_goal-y0 )
				{
					dx_prime = (y-y_goal) / dy;
				}
				else
				{
					dx_prime = (y0-y_goal) / 
							( 3*(y-y0)-(y1-y) );
					x = x0;
				}
				y1 = y;
			}
			else
			{
				x0 = x;
				if( y_goal-y <= y1-y_goal )
				{
					dx_prime = (y-y_goal) / dy;
				}
				else
				{
					dx_prime = (y1-y_goal) / 
							( 3*(y1-y)-(y-y0) );
					x = x1;
				}
				y0 = y;
			}
			
		// Skip if next bisection better than Newton's step
			if( abs(dx_prime) >= 0.125 )
				continue;
		
		// Find best bracket centered on f(xi) = y_goal
			x -= 2 * dx_prime * dx; 

			y = f(x);
			if( status || ::isnan(y) )
			{
				status = ZERO_INSIDE;
				return;
			}
			
			if( y == y_goal )
			{
				x0 = x1 = x;
				y0 = y1 = y;
				return;
			}
		}
		
	// Replace one of the bracket points with new point
		if( y < y_goal )
		{
			x0 = x;
			y0 = y;
		}
		else
		{
			x1 = x;
			y1 = y;
		}
	}

	status = ZERO_NOT_CONVERGED;
}



