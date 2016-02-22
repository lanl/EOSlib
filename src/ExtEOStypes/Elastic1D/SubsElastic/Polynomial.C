#include "Polynomial.h"
#include <LocalMath.h>
#include <iostream>
#include <iomanip>
using namespace std;

int Polynomial::Nmax = 8;
int Polynomial::ReInit(double *a, int n)
{
    if( n <= 0 )
    {
	    for( n=0;n<=Nmax; ++n)
	    {
	        if( isnan(a[n]) )
	            break;
	    }
        if( ! isnan(a[n]) )
        {
            delete [] A;
            N=0;
            return -1;
        }
    }
    if( n == 0 )
    {
        delete [] A;
        N=0;
        return 0;
    }
    if( n != N )
    {
        delete [] A;
        N = n;
        A = new double[N];
    }
    int i;
    for( i=0; i<N; i++)
        A[i] = a[i];
    return N;
}



Polynomial::~Polynomial()
{
    delete [] A;
}

ostream &Polynomial::print(ostream &out)
{
    if( N > 0 )
    {
        out << A[0];
        int i;
        for( i=1; i<N; i++ )
            out << ", " << A[i];
    }
    return out;
}

double Polynomial::f(double x)
{
    if( N < 1 )
        return 0.0;         // undefined
    double sum = A[N-1];
    for( int i=N-2; i>=0; i-- )
        sum = A[i] + x*sum;
    return sum;
}

double Polynomial::df(double x)
{
    if( N < 2 )
        return 0.0;
    double I = double(N-1);
    double sum = I*A[N-1];
    for( int i=N-2; i>0; i-- )
    {
        I -= 1.;
        sum = I*A[i] + x*sum;
    }
    return sum; // (d/dx) f(x)
}
double Polynomial::d2f(double x)
{
    if( N < 3 )
        return 0.0;
    double I = double(N-1);
    double sum = I*(I-1.)*A[N-1];
    for( int i=N-2; i>1; i-- )
    {
        I -= 1.;
        sum = I*(I-1.)*A[i] + x*sum;
    }
    return sum; // (d/dx)^2 f(x)
}
double Polynomial::d3f(double x)
{
    if( N < 4 )
        return 0.0;
    double I = double(N-1);
    double sum = I*(I-1.)*(I-2.)*A[N-1];
    for( int i=N-2; i>2; i-- )
    {
        I -= 1.;
        sum = I*(I-1.)*(I-2.)*A[i] + x*sum;
    }
    return sum; // (d/dx)^3 f(x)
}

double Polynomial::intf(double x)
{
    double I = double(N);
    double sum = A[N-1]/I;
    for( int i=N-2; i>=0; i-- )
    {
        I -= 1.;
        sum = A[i]/I + x*sum;
    }
    return sum*x; // int_0^x f(y)dy
}
