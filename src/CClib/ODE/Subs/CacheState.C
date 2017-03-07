#include <LocalMath.h>
#include <ODE.h>

CacheState::CacheState()
{
	y0 = y0_prime = dy = 0;
	c4 = 0;
	
	t0 = h = NaN;
}

CacheState::~CacheState()
{
	delete y0;
	delete y0_prime;
	delete dy;
	
	delete c4;
}

void CacheState::Initialize(int n)
{
	      y0 = new double[n];
	y0_prime = new double[n];
	      dy = new double[n];
	      
	 c4 = new double[n];
}


void CacheState::InitInterp(int n, double *dy_half, double *yp_1)
{
	double *yp_0 = y0_prime;
	double *Dy = dy;
	double *C4 = c4;
	
	double h2 = 2*h;
	
	while(n--)
		*C4++ = 8*(2*(*dy_half++) - (*Dy++))
				+ h2*( (*yp_1++) - (*yp_0++) );
		
}

void CacheState::Interpolate(int n, double *yp_1, double p,
						double *y, double *yp) const
{
//	Quartic Hermite interpolation: y0, y0_prime, y_half, y1_0, y1_prime	
//
//	p = (t-t0)/h,	and h = t1 - t0
//		t = (t0,t1) <-> p = (0,1)
//
//	y  = (1-p)^2 * ( y0 +   p   * (2*y0 + h*y0_prime) )
//	      + p^2  * ( y1 + (1-p) * (2*y1 - h*y1_prime) )
//	      + c4 * p^2 * (1-p)^2
//	   = y0 + p * (1-p)^2 * ( h*y0_prime )
//	      + p^2  * ( dy + (1-p) * (2*dy - h*y1_prime) )
//	      + c4 * p^2 * (1-p)^2
//
//	yp = 6*p*(1-p)*dy/h + (1-p)*  (1-3*p)    * y0_prime
//			    +   p  *(1-3*(1-p) ) * y1_prime
//			    + 2*(c4/h) * p*(1-p) * ( (1-p) - p ) 
//
//	ypp = (y1_prime - y0_prime)/h 
//			+ 6*(1-2p) * ( (y1-y0)/h - (y0_prime+y1_prime)/2 ) / h
//			+ 2*(c4/h^2) * (1 - 6*p*(1-p) )
//
//	yppp = -12 * ( (y1-y0)/h - (y0_prime+y1_prime)/2 ) / h^2
//			-12*(c4/h^3) *( (1-p) - p )
//
//	ypppp = 24*(c4/h^4)
//
//	where	c4 = 16 * (dy_half - 0.5 * dy) + 2*h * (y1_prime - y0_prime)
//
//	Note:
//	     If c4 = 0 then reduces to Cubic Hermite interpolation at end points
//
	double q = 1-p;
	double qq = q*q;
	double pp = p*p;
	
	double a1 =  pp*(1.+2*q);
	double a2 =  qq*p*h;
	double a3 = -pp*q*h;
	double a4 =  pp*qq;
	
	double b1 = 6*p*q/h;
	double b2 = q*(1-3*p);
	double b3 = p*(1-3*q);
	double b4 = 2*p*q*(q-p)/h;
	
	double *y_0 = y0;
	double *yp_0 = y0_prime;
	double *Dy = dy;
	double *C4 = c4;

	while(n--)
	{
		if( yp )
			*yp++ = b1*(*Dy) + b2*(*yp_0) + b3*(*yp_1) + b4*(*C4);
		
		*y++ = (*y_0++) + ( a1*(*Dy++) + a2*(*yp_0++) +  a3*(*yp_1++)
					+ a4 * (*C4++) );	
	}
}


int ODE::InitialState(double &t, double *y, double *yp)
{
	if( init )
	{
		t = Init.t0;
		CopyVec(y, Init.y0);
		if( yp )
			CopyVec(yp, Init.y0_prime);
	}
	return !init;
}

int ODE::LastState(double &t, double *y, double *yp)
{
	if( dir != 0 )
	{
		CacheState &last = Cache[cach1];
		t = last.t0;
		CopyVec(y, last.y0);
		if( yp )
			CopyVec(yp, last.y0_prime);
	}
	return dir;
}

