#include <iostream>
#include <Array.h>
#include <sstream>
using namespace std;

int main(int, char**)
{
        ostringstream teststream;
	string teststring =
	  string("Nums(-5) = 25\n") +
	  string("Nums(-4) = 26\n") +
	  string("Nums(-3) = 27\n") +
	  string("Nums(-2) = 28\n") +
	  string("Nums(-1) = 29\n") +
	  string("Nums(0) = 30\n") +
	  string("Nums(1) = 31\n") +
	  string("Nums(2) = 32\n") +
	  string("Nums(3) = 33\n") +
	  string("Nums(4) = 34\n") +
	  string("Nums(5) = 35\n");
	ARRAY<int> Nums(12);		// Twelve is the wrong guess but
					            // it should work anyhow
	int i;
	
	for(i=0; i < 257; i++)
		Nums[i] = i;
		
	Nums+=30;
	
	for(i=-5; i <= 5; i++)
		teststream << "Nums("<<i<<") = "<<Nums[i] << "\n";
	if (teststream.str() == teststring) return 0;
	return 1;
}
