#include <iostream>
#include <LocalMath.h>
#include <cstdlib>
using namespace std;

#include "ODE.h"


class Harmonic : public ODE
{
	double Omega2;
	double beta;
	double stop_value;
public:
	Harmonic(double Omeg, double b, double s = 0.5) : ODE(2,5)
	{
		Omega2 = Omeg;
		beta = b;
		stop_value = abs(s);
	}
	
	int F(double *y_dot, const double* y, double t);
	double StopCondition(const double *y, const double *yp, double t);
};


double Harmonic::StopCondition(const double *y, const double *, double)
{
	return Omega2*y[0]*y[0] + y[1]*y[1] - stop_value;
}

int Harmonic::F(double *y_dot, const double *y, double)
{
	y_dot[0] = y[1];
	y_dot[1] = -beta*y_dot[0] - Omega2*y[0];
	
	return 0;
}


int main(int, char**)
{
	double beta = 0.1;
	double Omega_2 = 1.0;
	Harmonic H(Omega_2, beta);
	double t = 0;
	
	H.epsilon = 1.e-8;
	//H.dt_max = 0.025;
	
	double delta = beta/2;
	double omega = sqrt(Omega_2 - delta*delta);
	
	double y[2];
	double dt = 1.e-10;
	double h = 0.01;
	
	y[0] = 1.0;
	y[1] = -delta;
	
	int status;
	
	status = H.Initialize(t, y, dt);
	dt = 0.1;
	
	H.IgnoreStopOn();
	H.CacheStep = 100;

	t = 100;
	H.max_steps = 10000;
	if( H.Integrate(t,y) )
	{
		cerr << "Error\n";
		exit(-1);
	}
	
	
	t=0;
	H.Integrate(t,y);
	H.IgnoreStopOff();
	
	for(t = 0; t < 100; t += dt)
	{
		status = H.Integrate(t,y);
		
		cout	<< t << " "
			<< y[0] << " "
			<< y[0] - exp(-delta*t)*cos(omega*t) << " "
			<< H.StepSize() << " "
			<< Omega_2*y[0]*y[0] + y[1]*y[1] << "\n";
		
		if(status)
			break;
	}
	
	
	if(status == STOP_CONDITION)
	{
		cerr << "\nStopped on StopCondition\n";

		t *= 1+ 1e-6;	// avoid roundoff error
		if( (status = H.Integrate(t,y)) )
			cerr << "Bad status: " << status << "\n";
	}
	else if(status)
	{
		cerr << "Stopped on Error\n";
		exit(-1);
	}
	else
		t -= dt;
		
	cout << "\n";
		
	status = H.Initialize(t, y, -dt);
//	H.IgnoreStopOff();
	
	for( int n = (int)(t/dt) ; n >=0 ; n--)
	{
		t = n*dt;
		status = H.Integrate(t,y);
		
		cout	<< t << " "
			<< y[0] << " "
			<< y[0] - exp(-delta*t)*cos(omega*t) << " "
			<< H.StepSize() << " "
			<< Omega_2*y[0]*y[0] + y[1]*y[1] << "\n";
		
		if(status)
			break;
	}

//	Round 2

	cout << "\nRound 2\n\n";
	
	Harmonic H2(Omega_2, beta, 1.5);
	H2.epsilon = 1.e-8;
	
	y[0] = 1.0;
	y[1] = -delta;
	t = 0;
	
	H2.Initialize(t, y, dt);
	
	do
	{
		t = -10.0;
		H2.Integrate(t, y);
		
		cerr << "Stopped at t = " << t << "\n";
		
	} while(t != -10.0);
	
	for(t = -10; t < 0; t += dt)
	{
		status = H2.Integrate(t,y);
		
		cout	<< t << " "
			<< y[0] << " "
			<< y[0] - exp(-delta*t)*cos(omega*t) << " "
			<< H2.StepSize() << " "
			<< Omega_2*y[0]*y[0] + y[1]*y[1] << "\n";
		
		if(status)
			break;
	}
	
	
	if(status == STOP_CONDITION)
		cerr << "\nStopped on StopCondition\n";
	else if(status)
	{
		cerr << "Stopped on Error\n";
		exit(-1);
	}

	return 0;
}
