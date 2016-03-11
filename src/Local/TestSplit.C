#include <LocalIo.h>
#include "String.h"
using namespace std;

#define MAX 64

int main(int argc, char** argv)
{
	if( argc != 2 )
		cerr << Error("need one and only argument with string") << Exit;
	SplitString string(argv[1]);
	
	char *sep = new char[64];
	while( 1 )
	{
		cout << "Type next separator: ";
		if( !(cin >> sep) )
			sep = NULL;
		const char *word = string.WordTo(sep);
		cout << "word:\t\t" << word << "\n";
		if( string.Remainder() == NULL )
		    return 0;
		cout << "remainder:\t" << string.Remainder() << "\n";
	}

	return 0;
}
