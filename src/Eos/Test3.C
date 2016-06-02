#include <LocalIo.h>
#include "EOS.h"
#include <iostream>
#include <stdlib.h>

using namespace std;
int main(int, char **argv)
{
        char *file = new char[128];
	file = strcpy(file, getenv("EOSLIB_DATA_PATH"));
	file = strcat(file, "/test_data/EosTest.data");
	DataBase db;
	if( db.Read(file) )
		cerr << Error("Read failed" ) << Exit;

    // load EOS_info()
    // but generates error since EOS:EOS::NULL does not exist
	cout << "got here\n"; cout.flush();
	EOS *eos = FetchEOS("EOS","NULL",db);
    if( eos )
        cerr << Error("fetch base class suceeded") << Exit;

    const char *info = db.BaseInfo("EOS");
    if( info == NULL )
		cerr << Error("No EOS info" ) << Exit;

    cout << info << "\n";

    cout << "sizeof(EOSbase) = " << sizeof(EOSbase) << "\n";
    cout << "sizeof(EOS) = " << sizeof(EOS) << "\n";
    delete[] file;
    return 0;
}
