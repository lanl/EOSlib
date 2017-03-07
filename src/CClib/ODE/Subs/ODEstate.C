#include <LocalMath.h>
#include <ODE.h>


ODEstate::ODEstate()
{
	y0 = y0_prime = 0;
	t0 = h = stop_condition = NaN;
}

ODEstate::~ODEstate()
{
	delete y0;
	delete y0_prime;
}

void ODEstate::Initialize(int n)
{
	      y0 = new double[n];
	y0_prime = new double[n];
}
