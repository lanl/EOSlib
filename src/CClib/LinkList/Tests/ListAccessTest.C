#include <iostream>
#include <sstream>
using namespace std;
#include "ListAccess.h"


class Double
{
	double dbl;
public:
	Double() { dbl = 0.0; }
	Double(double x) { dbl = x;}
	Double& operator=(double x) { dbl = x; return *this; }
	friend ostream& operator<<(ostream& s, Double&);
	operator double()	{ return dbl; }
};

inline ostream& operator<<(ostream& s, Double& d)
{
	s << d.dbl;
	return s;
}

int main(int, char**)
{
        ostringstream teststream;
	string teststring = "3\n2\n1\n";
	  
        Double a(1.0);
	Double b(2.0);
	Double c(3.0);
	Double *p;
	
	LIST<Double> A;
	
	A.Insert(a);
	A.Insert(b);
	A.Insert(c);
	
	LISTMANIP<Double> ManipA(A);
	
	
	for(A.Start(); (p = A); A++)
	{
		teststream << *ManipA.Get() << "\n";
	}

	if (teststream.str() == teststring) return 0;
	return -1;
}
