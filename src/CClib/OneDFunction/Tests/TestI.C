#include <LocalMath.h>
#include <Arg.h>
#include <iostream>

#include "OneDFunction.h"

using namespace std;
class Array : public OneDFunctionI
{
	int n;
	double *array;
public:
	Array(int dim)
	{
		n = dim;
		array = new double[dim];
	}
	~Array() { delete array; }
	
	//double f(int i) { return array[i];}
	double f(int i) { return i*i - 10; }
	
	double &operator[](int i) { return array[i]; }
	
};

int main(int, char **argv)
{
	Array A(100);
	
	int x0 = 0;
	int x1 = 50;
	int guess = HUGE_INT;
	double y_goal = 0.0;
	
	while(*++argv)
	{
		GetVar2(bracket, x0, x1);
		GetVar(guess, guess);
		GetVar(y,y_goal);
		ArgError;
	}

	for( int i=0; i<100; i++)
	{
		A[i] = (i-50)*(i-50) - 10;
	}
	
	A.inverse(y_goal, x0,x1,guess);
	
	cout << "Status: " << A.ErrorStatus() << "\n";
	
	cout << "Iterations = " << A.Iterations() << "\n";
	
	cout << A.x0 << " " << A.y0 << " 	 "
		<< A.x1 << " " << A.y1 << "\n";
	
	return 0;
}
