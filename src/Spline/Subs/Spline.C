#include "Spline.h"

Spline::~Spline()
{
	delete [] X;
	delete [] F;
	delete [] F2;
}

int Spline::ReInit( int n, double *&x, double *&f, double fp_0, double fp_1)
{
	delete [] X;
	delete [] F;
	X = x;
	F = f;
	x = f = nullptr;
    if( N != n )
    {
  	   N = n;
	   delete [] F2;
	   F2 = new double[N];
    }
	
	return Init(fp_0, fp_1);
}

int Spline::ReInitCopy( int n, const double x[], const double f[],
                               double fp_0, double fp_1)
{
	if( N != n)
	{	
		delete [] X;
		delete [] F;
		delete [] F2;
		N = n;
		X  = new double[N];
		F  = new double[N];
		F2 = new double[N];
	}
	int i;
	for(i=0; i<N; i++)
	{
		X[i] = x[i];
		F[i] = f[i];
	}
	return Init(fp_0, fp_1);
}

int Spline::Init(double fp_0, double fp_1)
{	
//	fp_0, fp_1 are derivatives at end points
//	if derivative = NaN then second derivative equals 0 (natural spline)

	// compute F2; from Numerical Recipes, chpt. 3, p. 88
	
	int i;
	double *U = new double[N];	// decomposition for tridiagonal matrix
	
	if( isnan(fp_0) )
	{  // natural boundary
		F2[0] = 0.;
		 U[0] = 0.;
	}
	else
	{
		F2[0] = -0.5;
		 U[0] = (3./(X[1]-X[0]))*((F[1]-F[0])/(X[1]-X[0])-fp_0);
	}
	
	for( i = 1; i < N-1; i++ )
	{ // decomposition of tridiagonal matrix
		double sig, p;
		sig = (X[i]-X[i-1])/(X[i+1]-X[i-1]);
		p = sig*F2[i-1]+2.;
		F2[i] = (sig-1.)/p;
		U[i] = ( 6.*( (F[i+1]-F[i])/(X[i+1]-X[i])
			- (F[i]-F[i-1])/(X[i]-X[i-1]) ) / (X[i+1]-X[i-1])
							- sig*U[i-1] )/p;
	}
	
	double Q1, U1;
	if( isnan(fp_1) )
	{  // natural boundary
		Q1 = 0.;
		U1 = 0.;
	}
	else
	{
		Q1 = 0.5;
		U1 = (3./(X[N-1]-X[N-2])) *
				(fp_1 - (F[N-1]-F[N-2])/(X[N-1]-X[N-2]));
	}

	F2[N-1] = (U1-Q1*U[N-2])/(Q1*F2[N-2]+1.);
	for( i = N-2; i >= 0; i-- )
	{  // Backwards substitution
		F2[i] = F2[i]*F2[i+1] + U[i];
	}
	
	delete [] U;
	return 0;
}

int Spline::Interval(double x) const
{
	// returns index of interval containing x,
	//	either X[i] <= x <= X[i+1] or X[i] >= x >= X[i+1]
			
	int i0 = 0;
	int i1 = N-1;
	
	int i = N/2;

	// binary search
	if( X[N-1] > X[0] )
	{	// x monotonically increasing
		if( x < X[0] || x > X[N-1] )
			return -1;
		if( N == 2 )
			return 0;

		while( i1-i0 > 1 )
		{
			if( x == X[i] )
			{
				i0 = (i < N-1) ? i: i-1 ;
				break;
			}
				
			if( x > X[i] )
				i0 = i;
			else
				i1 = i;
				
			i = (i0+i1)/2;
		}
	}
	else
	{	// x monotonically decreasing
		if( x > X[0] || x < X[N-1] )
			return -1;
		if( N == 2 )
			return 0;

		while( i1-i0 > 1 )
		{
			if( x == X[i] )
			{
				i0 = (i < N-1) ? i: i-1 ;
				break;
			}
				
			if( x < X[i] )
				i0 = i;
			else
				i1 = i;
				
			i = (i0+i1)/2;
		}
	}
	
	return i0;
}
double Spline::Evaluate(double x) const
{

	int i = Interval(x);
	if( i < 0 )
		return NaN;
		
	double dx = X[i+1] - X[i];
	double x1 = (X[i+1]- x)/dx;
	double x0 = (x-X[i])/dx;
	
	return x1*F[i] + x0*F[i+1]
		+ ( x1*(x1*x1-1.)*F2[i] + x0*(x0*x0-1.)*F2[i+1] ) *dx*dx/6.;

}

double Spline::Evaluate1(double x) const
{
	int i = Interval(x);
	if( i < 0 )
		return NaN;
		
	double dx = X[i+1] - X[i];
	double x1 = (X[i+1]- x)/dx;
	double x0 = (x-X[i])/dx;
    
    return (F[i+1]-F[i])/dx +
		( -(3*x1*x1 - 1.)*F2[i] + (3*x0*x0 - 1.)*F2[i+1] ) * dx/6.;
		
}

double Spline::Evaluate2(double x) const
{
	int i = Interval(x);
	if( i < 0 )
		return NaN;		
    return  ((X[i+1]-x)*F2[i] + (x-X[i])*F2[i+1])/(X[i+1]-X[i]);		
}

int Spline::Evaluate(double x, double f[3]) const
{
	int i = Interval(x);
	if( i < 0 )
		return -1;
		
	double dx = X[i+1] - X[i];
	double x1 = (X[i+1]- x)/dx;
	double x0 = (x-X[i])/dx;
	
	f[0] = x1*F[i] + x0*F[i+1]
		+ ( x1*(x1*x1-1.)*F2[i] + x0*(x0*x0-1.)*F2[i+1] ) * dx*dx/6.;
	
	f[1] = (F[i+1]-F[i])/dx +
		( -(3*x1*x1 - 1.)*F2[i] + (3*x0*x0 - 1.)*F2[i+1] ) * dx/6.;
		
	f[2] = x1*F2[i] + x0*F2[i+1];
	
	return 0;
}

int SplineWithInverse::InverseInterval(double f) const
{
	// returns index of interval containing x,
	//	either F[i] <= f <= F[i+1] or F[i] >= f >= F[i+1]
			
	int i0 = 0;
	int i1 = N-1;
	
	int i = N/2;

	// binary search
	if( F[N-1] > F[0] )
	{	// f monotonically increasing
		if( f < F[0] || f > F[N-1] )
			return -1;
		if( N == 2 )
			return 0;

		while( i1-i0 > 1 )
		{
			if( f == F[i] )
			{
				i0 = (i < N-1) ? i: i-1 ;
				break;
			}
				
			if( f > F[i] )
				i0 = i;
			else
				i1 = i;
				
			i = (i0+i1)/2;
		}
	}
	else
	{	// f monotonically decreasing
		if( f > F[0] || f < F[N-1] )
			return -1;
		if( N == 2 )
			return 0;

		while( i1-i0 > 1 )
		{
			if( f == F[i] )
			{
				i0 = (i < N-1) ? i: i-1 ;
				break;
			}
				
			if( f < F[i] )
				i0 = i;
			else
				i1 = i;
				
			i = (i0+i1)/2;
		}
	}
	
	return i0;
}

double SplineWithInverse::Inverse(double f)
{
	int i = InverseInterval(f);
	if( i < 0 )
		return NaN;

	if( f == F[i] )
		return X[i];
	if( f == F[i+1] )
		return X[i+1];
	
	double x;
	double dx = X[i+1] - X[i];
	double x0, x1;
	
	double fp0 = (F[i+1]-F[i])/dx - 2*F2[i]*dx/6.;
	double fp1 = (F[i+1]-F[i])/dx + 2*F2[i+1]*dx/6.;
	
	double f3 = (F2[i+1]-F2[i])/(X[i+1]-X[i]);
	double x_e = -F2[i]/f3;		// local extreme of fp (1st derivative)
	double x_end;
	
	if( x_e < X[i] || X[i+1] < x_e )
	{ // fp monotonic in interval
		if( fp0*fp1 < 0 )
			return NaN;	// F not monotonic
		if( std::abs(fp0) > std::abs(fp1) )
		{
			x = X[i];
			x_end = X[i+1];
		}
		else
		{
			x = X[i+1];
			x_end = X[i];
		}
	}
	else
	{
		x0 = (x_e-X[i])/dx;
		x1 = (X[i+1]- x_e)/dx;
		double fpe = (F[i+1]-F[i])/dx
		      + (-(3*x1*x1 - 1.)*F2[i] + (3*x0*x0 - 1.)*F2[i+1])*dx/6.;
		if( fp0*fpe < 0 || fpe*fp1 < 0 )
			return NaN;	// not monotonic
		double fx_e = x1*F[i] + x0*F[i+1]
		     + (x1*(x1*x1-1.)*F2[i] + x0*(x0*x0-1.)*F2[i+1])*dx*dx/6.;
		if( f == fx_e )
			return x_e;
		else if( min(F[i],fx_e ) < f && f < max(F[i],fx_e) )
		{ // f in interval (X[i],x_e)
		  if( std::abs(fp0) > std::abs(fpe) )
			{
				x = X[i];
				x_end = x_e;
			}
			else
			{
				x = x_e;
				x_end = X[i];
			}
		}
		else
		{ // f in interval (x_e,X[i+1])
		  if( std::abs(fp1) > std::abs(fpe) )
			{
				x = X[i+1];
				x_end = x_e;
			}
			else
			{
				x = x_e;
				x_end = X[i+1];
			}
		}	
	}
	
	
	double eps_f = max(abs_tol, rel_tol*std::abs(f));
	double eps_x = rel_tol*max(std::abs(X[i]),std::abs(X[i+1]));
	
	double fx, fpx, df;
	for( count=0; count < 60 ; count++ )
	{ // Newton iteration
		x0 = (x-X[i])/dx;
		x1 = (X[i+1]- x)/dx;
		fx = x1*F[i] + x0*F[i+1]
		     + (x1*(x1*x1-1.)*F2[i] + x0*(x0*x0-1.)*F2[i+1])*dx*dx/6.;

		fpx = (F[i+1]-F[i])/dx
		      + (-(3*x1*x1 - 1.)*F2[i] + (3*x0*x0 - 1.)*F2[i+1])*dx/6.;
			
		df = f - fx;
		double dx1;
		dx1 = df/fpx;
		x += dx1;
		if( std::abs(df) < eps_f && std::abs(dx1) < eps_x )
			return x;
	}
	
	// difficulty at end point if first and second derivative -> 0
	return x_end;
}

