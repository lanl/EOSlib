#include <LocalIo.h>
#include "EOS.h"
#include <iostream>

using namespace std;
int main(int, char **argv)
{
    const char *file = "Test.data";
	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;

    // load EOS_info()
    // but generates error since EOS:EOS::NULL does not exist
	EOS *eos = FetchEOS("EOS","NULL",db);
    if( eos )
        cerr << Error("fetch base class suceeded") << Exit;

    const char *info = db.BaseInfo("EOS");
    if( info == NULL )
		cerr << Error("No EOS info" ) << Exit;

    cout << info << "\n";

    cout << "sizeof(EOSbase) = " << sizeof(EOSbase) << "\n";
    cout << "sizeof(EOS) = " << sizeof(EOS) << "\n";
    return 0;
}
