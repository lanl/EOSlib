#include <LocalMath.h>
#include <OneDFunction.h>

using namespace std;

inline int round_up(double x)
{
	return (int)(x > 0.0 ? ceil(x) : floor(x));
}


void OneDFunctionI::PRE_ZERO(int x_guess)
{
	status = OK;
	
	y0 = f(x0);
	
	if( status )
	{
		status = ZERO_LOWER_LIMIT;
		return;
	}
	
	y1 = f(x1);
	
	if( status )
	{
		status = ZERO_UPPER_LIMIT;
		return;
	}
	
	PRE_CONDITION(x_guess);
}

void OneDFunctionI::PRE_CONDITION(int x_guess)
{
// Check for degenerate case	
	if( y0 == y_goal )
	{
		x1 = x0;
		y1 = y0;
		return;
	}
		
	if( y1 == y_goal )
	{
		x0 = x1;
		y0 = y1;
		return;
	}
		
// Check that limits form a bracket
	if( x0 == x1 )
	{
		status = ZERO_NO_BRACKET;
		return;
	}
	if( y0 > y_goal ? y1 > y_goal : y1 < y_goal )
	{
		status = ZERO_NO_BRACKET;
		return;
	}
	
// Interchange limits if necessary for y0 < y1
	if( y0 > y1 )
	{
		double temp = y0;
		y0 = y1;
		y1 = temp;
		
		int tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	
// Check for valid initial guess		
	if( x0 < x1 ? (x0 < x_guess && x_guess < x1) :
					(x1 < x_guess && x_guess < x0) )
	{
		status = OK;
	
	// evaluate at x_guess
		double y_guess = f(x_guess);
		if( status )
		{
			status = ZERO_INSIDE;
			return;
		}
		if( y_guess == y_goal )
		{
			x0 = x1 = x_guess;
			y0 = y1 = y_guess;
			return;
		}

	// Newton's method with unevenly spaced points	
		if( y0 < y_guess && y_guess < y1 )
		{
			double x_0 = x0;
			double x_1 = x1;
			double x_g = x_guess;
			
		// linearly interpolate derivative, same as quadratic fit
			double yp = ( (x_1-x_g) * (y0-y_guess)/(x_0-x_g)
				+ (x_g-x_0) * (y1-y_guess)/(x_1-x_g) )
					/ (x1 - x0);
				
			double dx = 2. * (y_guess - y_goal) / yp;
			int dx_prime = round_up(dx);
			
		
		// check trial within bounds	
			if( std::abs(dx_prime) < (y_guess > y_goal ?
					std::abs(x_guess-x0) : abs(x1-x_guess)) )
			{
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
				x_guess -= dx_prime;
					
				y_guess = f(x_guess);
				if( status )
				{
					status = ZERO_INSIDE;
					return;
				}
				if( y_guess == y_goal )
				{
					x0 = x1 = x_guess;
					y0 = y1 = y_guess;
					return;
				}
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
	
	return;
}


void OneDFunctionI::ZERO()
{
//
//	Iteratively improve brackets on zero crossing using
//	the best of either bisection or Newton's methods.
//	Assumes y0 < y_goal < y1
//
	int x, dx;
	double dx_prime;
	double y, dy;
		
	
	status = OK;
	for( cycles = 0; std::abs(dx = x1-x0) > 1; )
	{
		cycles++;
	// Compute function at center point of bracket
		x = (x0 + x1)/2;
		y = f(x);
		if( status )
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

	// Try Newton's method if |dx| > 4 and quadratic fit is monotonic
		//
		// y(x) = y + a * x - 4 b * x^2
		//	For equally spaced points x0, x, x1 scaled to (-0.5,0.5)
		// 	a = y1 - y0
		// 	b = y - 0.5 * (y1 + y0)
		// monotonic in (-0.5, 0.5) if |b/a| < 0.25
		//
		// check quadratic is monotonic
		//
		
		if( std::abs(dx) > 4 && abs(y - 0.5 * (y1 + y0)) < 0.25*(dy=y1-y0) )
		{
		// Replace one end point of bracket by (x, y)
		// and find new point x nearest to y_goal
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
			
		// Skip if next bisction better than Newton's step
			dx = round_up( 2.*dx_prime*dx );
			if( 2*std::abs(dx) > abs(x1-x0) )
			{
				continue;
			}
		
		// Find best bracket centered on f(xi) = y_goal
			x -= dx; 

			y = f(x);
			if( status )
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
}



