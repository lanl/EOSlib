#include <Array.h>
#include <iostream>
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
	
	
	pda->Xget_next() = one;
	pda->Xget_next() = two;
	pda->Xget_next() = three;
	
	cerr << "pda(0) = " << *pda->get(0)
		 << "  pda(1) = " << *pda->get(1)
		 << "  pda(2) = " << *pda->get(2)
		 << "\n";
	
	cerr << "pda(0) = " << *pda->Xget(0)
		 << "  pda(1) = " << *pda->Xget(1)
		 << "  pda(2) = " << *pda->Xget(2)
		 << "\n";
		
	cerr << "dimension = " << pda->Dimension() << "\n";
	
	if( ! pda->get(3) )
	{
		cerr << "get(3) -> 0\n";
	}
	else
		cerr << "Error get(3)\n";

	if( pda->Xget(3) )
	{
		cerr << "get(3) -> ! 0\n";
	}
	else
		cerr << "Error Xget(3)\n";
		
	*pda += 2;
	
	cerr << "Offset = " << pda->Offset() << "\n";
	cerr << "pda(0) = " << *pda->get(0)
		 << "  pda(-1) = " << *pda->get(-1)
		 << "  pda(-2) = " << *pda->get(-2)
		 << "\n";
	
	return 0;
}
