#include <iostream>
#include <LocalMath.h>

using namespace std;


int main()
{
	srand48(123456);
	unsigned short seed[3] = { 123, 456, 789};
	
	double x = drand48();
	
	double y = erand48(seed);
	
	long int number = lrand48();
	
	cerr << x << " " << y << " " << number << "\n";

	cerr << "NaN, " << NaN << "\n";

    cerr << "min " << Dlimits.min() << "\n";
    cerr << "max " << Dlimits.max() << "\n";
    //cerr << "lowest " << Dlimits.lowest() << "\n"; // C++11
    cerr << "epsilon "   << Dlimits.epsilon() << "\n";
    cerr << "HUGE_VAL "  << HUGE_VAL  << "\n";     // cmath
    cerr << "-HUGE_VAL " << -HUGE_VAL << "\n";     // cmath
    cerr << "HUGE "      << HUGE      << "\n";     // cmath
    cerr << "float HUGE_VAL " << numeric_limits<float>::max() << "\n";
    return 0;
}
