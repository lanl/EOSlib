#include "LocalIo.h"
#include "String.h"
#include <sstream>
using namespace std;

#define MAX 64

int main(int, char**)
{
        istringstream testin(std::string("3 Lor em Ipsum"));
	char *string;
	char *arg[5];
	int i;
	for( i=1; i<5; i++)
	     arg[i] = new char[MAX];
	while( 1 )
	{
		int n;
		// testout << "Type [1-3] arg1 ... argn: ";
		if( !(testin >> n) )
			break;
		// if( n < 1 || n > 4 )
		// {
		// 	cerr << "only 1 to 4 strings allowed, try again\n";
		// 	testin >> SkipTo("\n");
		// 	continue;
		// }

		switch(n)
		{
		  case 1:
			if( !(testin >> arg[1]) )
				cerr << Error("read failed") << Exit;
			string = Cat(arg[1]);
			break;
		  case 2:
			if( !(testin >> arg[1] >> arg[2]) )
				cerr << Error("read failed") << Exit;
			string = Cat(arg[1],arg[2]);
			break;
		  case 3:
			if( !(testin >> arg[1] >> arg[2] >> arg[3]) )
				cerr << Error("read failed") << Exit;
			string = Cat(arg[1],arg[2],arg[3]);
			break;
		  case 4:
			if( !(testin >> arg[1] >> arg[2] >> arg[3] >> arg[4]) )
				cerr << Error("read failed") << Exit;
			string = Cat(arg[1],arg[2],arg[3],arg[4]);
			break;
		  default:
		        cerr << Error("Program error") << Exit;
			
		}
		if(strcmp(string, "LoremIpsum") == 0) return 0;
		delete [] string;
	}

	return -1;
}
