#include <LocalIo.h>
#include "String.h"
using namespace std;

#define MAX 64

int main(int argc, char** argv)
{
        int out = 0;
        string test_string("Lorem Ipsum.");
	char* test_word = "Lorem";
	char* test_remainder = "Ipsum.";
	char* sep = " ";
	// if( argc != 2 )
	// 	cerr << Error("need one and only argument with string") << Exit;
	SplitString string(test_string.c_str());
	
	// char *sep = new char[64];
	const char *word = string.WordTo(sep);
	
	out -= strcmp(word, test_word);
	out -= strcmp(string.Remainder(), test_remainder);
	// while( 1 )
	// {
	// 	cout << "Type next separator: ";
	// 	if( !(cin >> sep) )
	// 		sep = NULL;
	// 	const char *word = string.WordTo(sep);
	// 	cout << "word:\t\t" << word << "\n";
	// 	if( string.Remainder() == NULL )
	// 	    return 0;
	// 	cout << "remainder:\t" << string.Remainder() << "\n";
	// }

	return out;
}
