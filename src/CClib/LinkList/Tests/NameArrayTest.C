#include "NameArray.h"
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
};

inline ostream& operator<<(ostream& s, Double& d)
{
	s << d.dbl;
	return s;
}

int main(int, char**)
{
        ostringstream teststream;
	string teststring =
	  string("TableDimensions = 3\n") +
	  string("Number of entries = 0\n") +
	  string("Number of entries = 6\n") +
	  string("12\n") +
	  string("1\n") +
	  string("14\n") +
	  string("15\n") +
	  string("16\n") +
	  string("17\n") +
	  string("fifth is defined\n") +
	  string("fifth is now undefined\n") +
	  string("Number of entries = 5\n") +
	  string("first\n") +
	  string("fourth\n") +
	  string("second\n") +
	  string("sixth\n") +
	  string("third\n");

	//  Test of new DoubleNameArray
	NAMEARRAY<Double> *pda = new NAMEARRAY<Double>(11);
	NAMEARRAY<Double> &da = *pda;
	//
	//DoubleNameArray da(3);
	//
	da.SetHashTableDimension(3);
	//	
	da.SetHashFunction(&_DefaultNameArrayHash); // optional
	//
	
	teststream << "TableDimensions = " << da.TableDimension() << "\n";
	teststream << "Number of entries = " << da.Dimension() << "\n";
	
	da["first"] = 12.0;
	da["second"] = 13.0;
	da["third"] = 14.0;
	da["fourth"] = 15.0;
	da["fifth"] = 16.0;
	da["sixth"] = 17.0;
	
	teststream << "Number of entries = " << da.Dimension() << "\n";
	teststream << da["first"] << "\n";
	teststream << da["second"] - da["first"] << "\n";
	teststream << da["third"] << "\n";
	teststream << da["fourth"] << "\n";
	teststream << da["fifth"] << "\n";
	teststream << da["sixth"] << "\n";
	
	if(da.Defined("fifth"))
		teststream << "fifth is defined\n";
	da.Delete("fifth");
	if(da.Defined("fifth"))
		teststream << "fifth is still defined\n";
	else
		teststream << "fifth is now undefined\n";
	
	teststream << "Number of entries = " << da.Dimension() << "\n";
	
	const char **Names = da.Names();
	int n = da.Dimension();
	int i;
	for(i=0; i<n; i++)
		teststream << Names[i] << "\n";
	
	delete [] Names;
	
	if (teststring == teststream.str()) return 0;
	return -1;
}
