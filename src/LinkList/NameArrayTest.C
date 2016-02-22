#include "NameArray.h"
#include <iostream>
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
	
	cout << "TableDimensions = " << da.TableDimension() << "\n";
	cout << "Number of entries = " << da.Dimension() << "\n";
	
	da["first"] = 12.0;
	da["second"] = 13.0;
	da["third"] = 14.0;
	da["fourth"] = 15.0;
	da["fifth"] = 16.0;
	da["sixth"] = 17.0;
	
	cout << "Number of entries = " << da.Dimension() << "\n";
	cout << da["first"] << "\n";
	cout << da["second"] - da["first"] << "\n";
	cout << da["third"] << "\n";
	cout << da["fourth"] << "\n";
	cout << da["fifth"] << "\n";
	cout << da["sixth"] << "\n";
	
	if(da.Defined("fifth"))
		cout << "fifth is defined\n";
	da.Delete("fifth");
	if(da.Defined("fifth"))
		cout << "fifth is still defined\n";
	else
		cout << "fifth is now undefined\n";
	
	cout << "Number of entries = " << da.Dimension() << "\n";
	
	const char **Names = da.Names();
	int n = da.Dimension();
	int i;
	for(i=0; i<n; i++)
		cout << Names[i] << "\n";
	
	delete [] Names;
	
	return 0;		
}
