#include <iostream>
#include <cmath>
#include <cstdlib>
using namespace std;

#include "ODE.h"


class Harmonic : public ODE
{
	double Omega2;
	double beta;
public:
	Harmonic(double Omeg, double b) : ODE(2)
	{
		Omega2 = Omeg;
		beta = b;
	}
	
	int F(double *y_dot, const double *y, double t);
	double StopCondition(const double *y, const double *yp, double t);
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
	double dt = 0.1;
	double h = 0.01;
	
	y[0] = 1.0;
	y[1] = -delta;
	
	int status;
	
	status = H.Initialize(t, y, 0.01);
	
	H.IgnoreStopOn();
	H.CacheStep = 100;

/*
	t = 100;
	if( status = H.Integrate(t,y) )
	{
		cerr << "Error " << status << "\n";
		exit(-1);
	}
*/
	H.IgnoreStopOff();
	
	for(t = 10; t > -10;)
	{
		status = H.Backward(t,y);
		
		cout	<< t << " "
			<< y[0] << " "
			<< y[0] - exp(-delta*t)*cos(omega*t) << " "
			<< H.StepSize() << " "
			<< Omega_2*y[0]*y[0] + y[1]*y[1] << "\n";
		
		if(status)
			break;
	}
	
	
	if(status == STOP_CONDITION)
		cerr << "\nStopped on StopCondition\n";
	else if(status)
	{
		cerr << "Stopped on Error " << status << "\n";
		exit(-1);
	}
	else
		t -= dt;

	//H.IgnoreStopOff();
		
	do
	{
		t = 10.;
		H.Integrate(t,y);
		cout << "stopped at " << t << "\n";
	} while( t != 10);
		
	cout << "\n";
		
	return 0;
}
