#include <iostream>
#include <LocalMath.h>
#include <LocalIo.h>
#include <cstdlib>

#include "ODE.h"

class Harmonic : public ODE
{
	double Omega2;
	double beta;
public:
	Harmonic(double Omeg, double b) : ODE(2,2)
	{
		Omega2 = Omeg;
		beta = b;
	}
	
	int F(double *y_dot, const double *y, double t);
	double StopCondition(const double *y, const double *yp, double t);
	int SizeOfCache() { return Initial.Dimension(); }
};



double Harmonic::StopCondition(const double *y, const double *, double)
{
	return Omega2*y[0]*y[0] + y[1]*y[1] - 0.5;
}

int Harmonic::F(double *y_dot, const double *y, double)
{
	y_dot[0] = y[1];
	y_dot[1] = -beta*y_dot[0] - Omega2*y[0];
	
	return 0;
}


class Analytic
{
	double Omega2;
	double beta;
	double delta;
	double omega;
	double t0;
	double C, S;
	
public:
	Analytic(double Omeg, double b);	
	void Initialize(double t, double *y);
	
	double Integrate(double t);
};
	

Analytic::Analytic(double Om, double bet)
{
	Omega2 = Om;
	beta = bet;
	delta = beta/2;
	omega = sqrt(Omega2 - delta*delta);
}

void Analytic::Initialize(double t, double *y)
{
	t0 = t;
	C = y[0];
	S = (y[1] + delta*y[0])/omega;
}

double Analytic::Integrate(double t)
{
	return(exp(-delta*(t - t0))*(C*cos(omega*t) + S*sin(omega*t)));
}
	
		


int main(int, char**)
{
	double beta = 0.1;
	double Omega_2 = 1.0;
	Harmonic H(Omega_2, beta);
	Analytic A(Omega_2, beta);
	
	double t0 = 0.0;
	
	
	H.epsilon = 1.e-8;
	//H.dt_max = 0.025;
	
	double delta = beta/2;
	double omega = sqrt(Omega_2 - delta*delta);
	
	double y[2];
	double dt = 0.1;
	double h = 0.01;
	
	y[0] = 1.0;
	y[1] = -delta;
	
	
	double t_min = 0.0;
	double t_max = 20.0;
	
	
	int status;
	
	status = H.Initialize(t0, y, dt);
	A.Initialize(t0, y);
	
	H.IgnoreStopOn();
	H.CacheStep = 10;

	double t;
	
	int n_try = 20000;
	double dy;
	
//	H.Integrate(1.0, y);
	
	while(n_try--)
	{
		if(!(n_try % 100) || n_try > 19990)
		  std::cerr << n_try << ": Size of Cache: " << H.SizeOfCache()
				<< "\n";
			
		t = t_min + drand48()*(t_max - t_min);
		
		H.Integrate(t, y);
		
		dy = y[0] - A.Integrate(t);
		
		if(std::abs(dy) > H.epsilon)
		  std::cerr << "Bad Case " << n_try << "  "<< t << " " << dy
				<< "\n";
	}
	return 0;
}



