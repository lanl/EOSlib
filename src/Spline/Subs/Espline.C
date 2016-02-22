#include "Spline.h"

int Espline::ReInitCopy(int n, double x0,   double x1, const double f[],
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
		X[i] = x0 + (double(i)/double(N-1))*(x1-x0);
		F[i] = f[i];
	}	
	return Init(fp_0, fp_1);
}

int Espline::ReInit(int n, double x0, double x1, double *&f,
                        double fp_0, double fp_1)
{
	if( N != n)
	{	
		delete [] X;
		delete [] F2;
		N = n;
		X  = new double[N];
		F2 = new double[N];
	}
    F = f;
    f = NULL;
	int i;
	for(i=0; i<N; i++)
		X[i] = x0 + (double(i)/double(N-1))*(x1-x0);
	return Init(fp_0, fp_1);
}

Espline::~Espline()
{ 
    // Null
}

int Espline::Interval(double x) const
{
    x = (x-X[0])/(X[N-1]-X[0]);
    if( x < 0 || x > 1 )
        return -1;
    return x < 1 ? int(x*(N-1)) : N-2;
}
