#include <Array.h>
#include <iostream>
#include <sstream>
using namespace std;

class Double
{
	double dbl;
public:
	Double& operator=(double x) { dbl = x; return *this; }
	friend ostream& operator<<(ostream& s, Double&);
	operator double()	{return dbl;}
	Double() { dbl = 0; }
	Double(double x) { dbl = x; }
};

inline ostream& operator<<(ostream& s, Double& d)
{
	s << d.dbl;
	return s;
}

int main(int, char**)
{
	//  Test of new DoubleArray
	ARRAY<Double> *pda = new ARRAY<Double>;
	pda->ArrayInit(10);
	
	Double one(1);
	Double two(2);
	Double three(3);
	ostringstream teststream;
	string teststring =
	  string("pda(0) = 1  pda(1) = 2  pda(2) = 3\n") +
	  string("pda(0) = 1  pda(1) = 2  pda(2) = 3\n") +
	  string("dimension = 3\n") +
	  string("get(3) -> 0\n") +
	  string("get(3) -> ! 0\n") +
	  string("Offset = 2\n") +
	  string("pda(0) = 3  pda(-1) = 2  pda(-2) = 1\n");
	
	
	pda->Xget_next() = one;
	pda->Xget_next() = two;
	pda->Xget_next() = three;
	
	teststream << "pda(0) = " << *pda->get(0)
		 << "  pda(1) = " << *pda->get(1)
		 << "  pda(2) = " << *pda->get(2)
		 << "\n";
	
	teststream << "pda(0) = " << *pda->Xget(0)
		 << "  pda(1) = " << *pda->Xget(1)
		 << "  pda(2) = " << *pda->Xget(2)
		 << "\n";
		
	teststream << "dimension = " << pda->Dimension() << "\n";
	
	if( ! pda->get(3) )
	{
		teststream << "get(3) -> 0\n";
	}
	else
		teststream << "Error get(3)\n";

	if( pda->Xget(3) )
	{
		teststream << "get(3) -> ! 0\n";
	}
	else
		teststream << "Error Xget(3)\n";
		
	*pda += 2;
	
	teststream << "Offset = " << pda->Offset() << "\n";
	teststream << "pda(0) = " << *pda->get(0)
		 << "  pda(-1) = " << *pda->get(-1)
		 << "  pda(-2) = " << *pda->get(-2)
		 << "\n";
	if( teststream.str() == teststring) return 0;
	
	return -1;
}
