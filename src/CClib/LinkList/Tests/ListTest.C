// test program of Linked List

#include "List.h"

//

#include <iostream>
#include <sstream>
using namespace std;


class Dbl
{
public:
	double x;
};


int main(int, char**)
{
    string teststring =      
      string("\nBegin Test\n") +
      string("error status = 0\n") +
      string("error status = 1\n") +
      string("error status = 0\n") +
      string("if is true \n") +
      string("error status = 0\n") +
      string("if is true \n") +
      string("error status = 2\n");

    ostringstream teststream;
    
    Dbl x1, x2, x3;
	LIST<Dbl> T;
	FILO<Dbl> Tstack;
	FIFO<Dbl> Tpipe;
	
	teststream << "\nBegin Test\n";
	
	
	_List *list = (_List*)(&T);
	
								// error status
	Tstack << x1;						// 0

	teststream << "error status = " << Tstack.Status() << "\n";
	
	Tstack >> x1 >> x2;					// 1
	
	teststream << "error status = " << Tstack.Status() << "\n";
	
	Tstack.Reset();						// 0
	
	teststream << "error status = " << Tstack.Status() << "\n";
	
	
	if( Tstack >> x1 )					// 1 / 0
		teststream << "if is true \n";
	else
		teststream << "if is false\n";
		
	teststream << "error status = " << Tstack.Status() << "\n";	// 0
			
	if( Tstack >> x1 >> x2, Tstack.Status() )		// 1
		teststream << "if is true \n";
	else
		teststream << "if is false\n";
		
	teststream << "error status = " << Tstack.Status() << "\n";	// 1
	
	if (teststream.str() == teststring) return 0;
	return 1;
}
