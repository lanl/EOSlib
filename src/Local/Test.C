#include "Arg.h"

int main(int, char** argv)
{
	ProgName(*argv);
	
	double x = 0;
	double y = 0;
	
	while(*++argv)
	{
		GetVar(X, x);
		GetVar(Y, y);
		ArgError;
	}
	
	std::cout << "X = " << x <<"\t" << "Y = " << y << "\n";

	return 0;
}
