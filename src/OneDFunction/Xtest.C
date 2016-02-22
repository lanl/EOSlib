#include <Arg.h>
#include "OneDFunction.h"

class Funct : public OneDFunction
{
public:
	double f(double x) { return atan(x);}
	
};

int main(int, char **argv)
{
	Funct A;
	
	double x0 = -5;
	double x1 = 5;
	double guess=HUGE;
	double y_goal = 0.0;
	
	while(*++argv)
	{
		GetVar2(bracket, x0, x1);
		GetVar(guess, guess);
		GetVar(y,y_goal);
		ArgError;
	}
	
	double x = A.inverse(y_goal, x0,x1,guess);
	
	cout << "Status: " << A.ErrorStatus()
		<< "  x, y = " << x << " " << y_goal << "\n";
	
	cout << "Iterations = " << A.Iterations() << "\n";
	
	cout << A.x0 << " " << A.y0 << " 	 "
		<< A.x1 << " " << A.y1 << "\n";
	
	return 0;
}
