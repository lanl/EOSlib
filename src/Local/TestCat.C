#include "LocalIo.h"
#include "String.h"
using namespace std;

#define MAX 64

int main(int, char**)
{
	char *string;
	char *arg[5];
	int i;
	for( i=1; i<5; i++)
	     arg[i] = new char[MAX];
	while( 1 )
	{
		int n;
		cout << "Type [1-3] arg1 ... argn: ";
		if( !(cin >> n) )
			break;
		if( n < 1 || n > 4 )
		{
			cerr << "only 1 to 4 strings allowed, try again\n";
			cin >> SkipTo("\n");
			continue;
		}

		switch(n)
		{
		  case 1:
			if( !(cin >> arg[1]) )
				cerr << Error("read failed") << Exit;
			string = Cat(arg[1]);
			break;
		  case 2:
			if( !(cin >> arg[1] >> arg[2]) )
				cerr << Error("read failed") << Exit;
			string = Cat(arg[1],arg[2]);
			break;
		  case 3:
			if( !(cin >> arg[1] >> arg[2] >> arg[3]) )
				cerr << Error("read failed") << Exit;
			string = Cat(arg[1],arg[2],arg[3]);
			break;
		  case 4:
			if( !(cin >> arg[1] >> arg[2] >> arg[3] >> arg[4]) )
				cerr << Error("read failed") << Exit;
			string = Cat(arg[1],arg[2],arg[3],arg[4]);
			break;
		  default:
			cerr << Error("Program error") << Exit;
			
		}
		cout << string << "\n";
		delete [] string;
	}

	return 0;
}
