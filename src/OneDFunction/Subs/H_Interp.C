#include <OneDFunction.h>


void HermiteInterpolate::Init(double y_0, double del_y,
		double y_0_prime, double y_1_prime, double x0, double delx)
{
	x_0 = x0;
	h = delx;
	
	a_0 = y_0;
	a_1 = y_0_prime*h;
	a_2 = 3*del_y - h*(2*y_0_prime + y_1_prime);
	a_3 = -2*del_y + h*(y_0_prime + y_1_prime);
	
}


int HermiteInterpolate::Eval(double x, double &y)
{

	if(x+h < x_0 || x-2*h > x_0)
	{
		y = RelEval(x < x_0 ? -1 : 2);
		return FAILED;
	}
		
	if(x == x_0)
	{
		y = a_0;
		return INSIDE;
	}
	
	y = RelEval((x - x_0)/h);
	
	return x < x_0 ? BELOW : x <= x_0 + h ? INSIDE : ABOVE;
}





void HermiteInterpolateN::Init(double *y_0, double *del_y,
		double *y_0_prime, double *y_1_prime, double x0, double delx)
{
	x_0 = x0;
	h = delx;
	
	double *c_0 = a_0;
	double *c_1 = a_1;
	double *c_2 = a_2;
	double *c_3 = a_3;
	
	int i = n_dim;
	
	while(i--)
	{
		*c_0++ = *y_0++;
		*c_1++ = (*y_0_prime)*h;
		*c_2++ = 3*(*del_y) - h*(2*(*y_0_prime) + (*y_1_prime));
		*c_3++ = -2*(*del_y++) + h*((*y_0_prime++) + (*y_1_prime++));
	}
	
	init = 1;

}

int HermiteInterpolateN::Eval(double x, double *y, double *yp)
{
	if(!init)
		return FAILED;
		
	if(x == x_0)
	{
		double *c_0 = a_0;
		int i = n_dim;
		
		while(i--)
			*y++ = *c_0++;
		return INSIDE;
	}
	
	if( h == 0 )
		return FAILED;
		
	double p = (x - x_0)/h;
		
	if( p < -1 || p > 2 )
	{
		rel_val( (p < -1) ? -1 : 2, y);
		return FAILED;
	}
		
	
	rel_val(p, y, yp);
	
	return (p < 0) ? BELOW : (p <= 1) ? INSIDE : ABOVE;
}


void HermiteInterpolateN::rel_val(double p, double *y, double *yp)
{
		
	double *c_0 = a_0;
	double *c_1 = a_1;
	double *c_2 = a_2;
	double *c_3 = a_3;
	
	int i = n_dim;
	
	while(i--)
	{
		if( yp )
			*yp++ = ((3*(*c_3) * p + 2*(*c_2)) * p + (*c_1))/h;
						
		*y++ = (((*c_3++) * p + (*c_2++)) * p +
						(*c_1++)) * p + *c_0++;
	}
}


	
void HermiteInterpolateN::Allocate(int n)
{
	if(n_dim > 0 && n_dim != n)
	{
		delete a_0;
		delete a_1;
		delete a_2;
		delete a_3;
		n_dim = 0;
	}
	
	if(!n_dim)
	{
		a_0 = new double[n];
		a_1 = new double[n];
		a_2 = new double[n];
		a_3 = new double[n];
	}
	
	n_dim = n;
	init = 0;
	
}

