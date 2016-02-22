// test program of Linked List

#include "List.h"

//

#include <iostream>
using namespace std;


class Dbl
{
public:
	double x;
};


int main(int, char**)
{
    Dbl x1, x2, x3;
	LIST<Dbl> T;
	FILO<Dbl> Tstack;
	FIFO<Dbl> Tpipe;
	
	cout << "\nBegin Test\n";
	
	
	_List *list = (_List*)(&T);
	
								// error status
	Tstack << x1;						// 0

	cout << "error status = " << Tstack.Status() << "\n";
	
	Tstack >> x1 >> x2;					// 1
	
	cout << "error status = " << Tstack.Status() << "\n";
	
	Tstack.Reset();						// 0
	
	cout << "error status = " << Tstack.Status() << "\n";
	
	
	if( Tstack >> x1 )					// 1 / 0
		cout << "if is true \n";
	else
		cout << "if is false\n";
		
	cout << "error status = " << Tstack.Status() << "\n";	// 0
			
	if( Tstack >> x1 >> x2, Tstack.Status() )		// 1
		cout << "if is true \n";
	else
		cout << "if is false\n";
		
	cout << "error status = " << Tstack.Status() << "\n";	// 1
	
	return 0;		
}
